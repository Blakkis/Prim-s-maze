#include "w_game.hpp"
#include "w_utils.hpp"
#include "w_timer.hpp"

#include <iostream>
#include <vector>
#include <ctime>

// Based on:
// http://weblog.jamisbuck.org/2011/1/10/maze-generation-prim-s-algorithm

// Keep the cell size power-of-two
const int cell_size = 16;


// No instance version
void w_drawLine(SDL_Renderer *wRenderer, SDL_Texture *target, int x1, int y1, int x2, int y2, uint32_t color)
{
    auto rgba = uint32ToVec4(color);
    if(rgba.w < 0xff) SDL_SetRenderDrawBlendMode(wRenderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(wRenderer, target);
    SDL_SetRenderDrawColor(wRenderer, rgba.x, rgba.y, rgba.z, rgba.w);

    SDL_RenderDrawLine(wRenderer, x1, y1, x2, y2);

    // Reset back to default renderer
    if(target) SDL_SetRenderTarget(wRenderer, nullptr);
}


static bool within(int v, int v_min, int v_max)
{
    if (v > v_min && v < v_max) return true;
    return false;
}


// Fetch random done cell near candidate
static std::pair<int, int> valid_dones(std::vector<std::vector<int>> &p_grid, int x, int y)
{
    std::vector<std::pair<int, int>> mov;

    const std::vector<std::pair<int, int>> dirs = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    for (const auto &f: dirs){
        if (within(y + f.second, -1, p_grid.size()) &&
            within(x + f.first,  -1, p_grid[0].size()) &&
            p_grid[y + f.second][x + f.first] == 1){

            mov.push_back(std::make_pair(x + f.first, y + f.second));
        }
    }
    int rdir = rand() % mov.size();

    return mov[rdir];
}


// Advance to next cell
static void advance(std::vector<std::vector<int>> &p_grid, std::vector<std::pair<int, int>> &p_done,
                    std::vector<std::pair<int, int>> &p_cand, int rx, int ry)
{
    p_grid[ry][rx] = 1;
    p_done.push_back(std::make_pair(rx, ry));

    const std::vector<std::pair<int, int>> dirs = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    for(const auto &f: dirs){
        if (within(ry + f.second, -1, p_grid.size()) &&
            within(rx + f.first,  -1, p_grid[0].size()) &&
            p_grid[ry + f.second][rx + f.first] == 0){

            p_grid[ry + f.second][rx + f.first] = 2;
            p_cand.push_back(std::make_pair(rx + f.first, ry + f.second));
        }
    }
}


// Do the frontier line segments
static void advanceDraw(std::vector<std::vector<int>> &p_grid, SDL_Texture *maze,
                        std::pair<int, int> to, std::pair<int, int> from, SDL_Renderer *wRenderer)
{
    const int max_x = p_grid[0].size();
    const int max_y = p_grid.size();

    const int x2 = to.first;
    const int y2 = to.second;

    const int x2_mult = x2 * cell_size;
    const int y2_mult = y2 * cell_size;

    // Note: Combine these to 1 for loop like above for better editing in the future
    if (within(y2 - 1, -1, max_y) && p_grid[y2 - 1][x2] == 1 && y2 - 1 != from.second){
        w_drawLine(wRenderer, maze, x2_mult,
                                    y2_mult,
                                    x2_mult + cell_size,
                                    y2_mult,
                                    0xffffffff);
    }

    if (within(y2 + 1, -1, max_y) && p_grid[y2 + 1][x2] == 1 && y2 + 1 != from.second){
        w_drawLine(wRenderer, maze, x2_mult,
                                    y2_mult + cell_size,
                                    x2_mult + cell_size,
                                    y2_mult + cell_size,
                                    0xffffffff);
    }

    if (within(x2 - 1, -1, max_x) && p_grid[y2][x2 - 1] == 1 && x2 - 1 != from.first){
        w_drawLine(wRenderer, maze, x2_mult,
                                    y2_mult,
                                    x2_mult,
                                    y2_mult + cell_size,
                                    0xffffffff);
    }

    if (within(x2 + 1, -1, max_x) && p_grid[y2][x2 + 1] == 1 && x2 + 1 != from.first){
        w_drawLine(wRenderer, maze, x2_mult + cell_size,
                                    y2_mult,
                                    x2_mult + cell_size,
                                    y2_mult + cell_size,
                                    0xffffffff);
    }


}

static bool primS(std::vector<std::vector<int>> &p_grid, SDL_Texture *maze,
                  bool set_spawn = false, SDL_Renderer *wRenderer = nullptr)
{
    static std::vector<std::pair<int, int>> already_done;
    static std::vector<std::pair<int, int>> candidates;

    if(set_spawn){
        int rx = rand() % (p_grid[0].size() - 1);
        int ry = rand() % (p_grid.size()    - 1);

        advance(p_grid, already_done, candidates, rx, ry);
    }
    else{
        // No more room to fill. Return done
        if (candidates.size() == 0) return true;

        int rcan = rand() % candidates.size();
        auto direction = valid_dones(p_grid, candidates[rcan].first, candidates[rcan].second);

        advance(p_grid, already_done, candidates, candidates[rcan].first, candidates[rcan].second);
        advanceDraw(p_grid, maze, candidates[rcan], direction, wRenderer);

        candidates.erase(candidates.begin() + rcan);
    }
    return false;
}


W_Game::W_Game(const char *title, int width, int height)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    wWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
    wRenderer = SDL_CreateRenderer(wWindow, -1, 0);

    wWidth = width;
    wHeight = height;
}


W_Game::~W_Game(void)
{
    SDL_DestroyWindow(wWindow);
    SDL_DestroyRenderer(wRenderer);

    SDL_Quit();
}


void W_Game::w_clear(uint32_t color)
{
    auto rgba = uint32ToVec4(color);
    SDL_SetRenderDrawColor(wRenderer, rgba.x, rgba.y, rgba.z, rgba.w);
    SDL_RenderClear(wRenderer);
}


void W_Game::w_flip(void)
{
    SDL_RenderPresent(wRenderer);
}


void W_Game::w_drawRect(SDL_Texture *target, int x1, int y1, int x2, int y2, uint32_t color, bool _fill)
{
    auto rgba = uint32ToVec4(color);
    if(rgba.w < 0xff) SDL_SetRenderDrawBlendMode(wRenderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(wRenderer, target);
    SDL_SetRenderDrawColor(wRenderer, rgba.x, rgba.y, rgba.z, rgba.w);

    const SDL_Rect dst{x1, y1, x2, y2};
    if(!_fill) SDL_RenderDrawRect(wRenderer, &dst);
    else SDL_RenderFillRect(wRenderer, &dst);

    // Reset back to default renderer
    if(target) SDL_SetRenderTarget(wRenderer, nullptr);
}


void W_Game::w_drawLine(SDL_Texture *target, int x1, int y1, int x2, int y2, uint32_t color)
{
    auto rgba = uint32ToVec4(color);
    if(rgba.w < 0xff) SDL_SetRenderDrawBlendMode(wRenderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(wRenderer, target);
    SDL_SetRenderDrawColor(wRenderer, rgba.x, rgba.y, rgba.z, rgba.w);

    SDL_RenderDrawLine(wRenderer, x1, y1, x2, y2);

    // Reset back to default renderer
    if(target) SDL_SetRenderTarget(wRenderer, nullptr);
}


void W_Game::w_gameloop(void)
{
    W_FramerateManager w_timer{.num_of_samples=16};

    srand(time(NULL));
    SDL_Texture *maze = SDL_CreateTexture(wRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, wWidth, wHeight);

    int x = 0, y = 0;
    for (y; y < wHeight / cell_size; y++){
        x = 0;
        for (x; x < wWidth / cell_size; x++){
            w_drawRect(maze, x * cell_size, y * cell_size, cell_size, cell_size, 0x101010ff);
        }
    }
    w_drawRect(maze, 0, 0, wWidth, wHeight, 0xffffffff);

    std::vector<std::vector<int>> grid_2d(y, std::vector<int>(x));
    primS(grid_2d, maze, true);

    SDL_Event event;

    double up = 0.0;
    bool draw_done = false;

    while(1)
    {
        w_timer.tick();
        SDL_SetWindowTitle(wWindow, std::to_string(w_timer.ft_getFramerate()).c_str());

        w_clear(0x303030ff);

        while(SDL_PollEvent(&event) != 0){
            switch(event.type){
                case SDL_QUIT: return;

                case SDL_KEYUP:
                    int key = event.key.keysym.sym;
                    switch(key){
                        case SDLK_UP:
                            primS(grid_2d, maze, false, wRenderer);
                    }
            }
        }

        if (!draw_done){
            if (w_timer.ft_getSeconds() > up)
            {
                up = w_timer.ft_getSeconds() + .0002;
                draw_done = primS(grid_2d, maze, false, wRenderer);
            }
        }

        SDL_RenderCopy(wRenderer, maze, nullptr, nullptr);

        // Debug draw (Comment out to save framerate)
        // Conquered cell 1: White
        // Candidate cell 2: Green
        for (y = 0; y < wHeight / cell_size; y++){
            for (x = 0; x < wWidth / cell_size; x++){
                if (grid_2d[y][x] == 1){
                    w_drawRect(nullptr, x * cell_size, y * cell_size, cell_size, cell_size, 0xffffff20, true);
                }
                else if (grid_2d[y][x] == 2){
                    w_drawRect(nullptr, x * cell_size, y * cell_size, cell_size, cell_size, 0x00400080, true);
                }
            }
        }

        w_flip();
    }
}
