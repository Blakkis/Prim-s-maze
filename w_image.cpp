#include "w_image.hpp"

#include "SDL_image.h"

#include <iostream>

W_Texture::W_Texture(SDL_Renderer *renderer, const std::string &texture_path,
                     int texture_rows, int texture_columns)
{
    SDL_Surface *temp = IMG_Load(texture_path.c_str());
    if(!temp){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load texture: %s", texture_path.c_str());
        return;
    }

    // Create the texture and provide default values for the texture rect
    t_texture = SDL_CreateTextureFromSurface(renderer, temp);
    t_texture_rect_dst.x = 0;
    t_texture_rect_dst.y = 0;

    t_texture_rect_src.x = 0;
    t_texture_rect_src.y = 0;

    t_texture_rect_dst.w = temp->w;
    t_texture_rect_dst.h = temp->h;

    // Store reference to main renderer
    t_renderer = renderer;

    if (texture_rows > 0){
        t_num_of_rows = temp->w / texture_rows;
        t_texture_rect_dst.w = t_num_of_rows;
        t_texture_rect_src.w = t_num_of_rows;
        t_max_index_x = texture_rows - 1;
    }
    else{
        t_texture_rect_src.w = temp->w;
    }

    if (texture_columns > 0){
        t_num_of_columns = temp->h / texture_columns;
        t_texture_rect_dst.h = t_num_of_columns;
        t_texture_rect_src.h = t_num_of_columns;
        t_max_index_y = texture_columns - 1;
    }
    else{
        t_texture_rect_src.h = temp->h;
    }

    SDL_FreeSurface(temp);

}

void W_Texture::render(int x, int y, bool outline, int row, int column)
{
    t_texture_rect_dst.x = x;
    t_texture_rect_dst.y = y;


    if (row != -1){
        t_texture_rect_src.x = t_num_of_rows * (row % t_max_index_x);
    }
    if (column != -1){
        t_texture_rect_src.y = t_num_of_columns * (column % t_max_index_y);
    }

    SDL_RenderCopy(t_renderer, t_texture, &t_texture_rect_src, &t_texture_rect_dst);

    if(outline){
        SDL_SetRenderDrawColor(t_renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderDrawRect(t_renderer, &t_texture_rect_dst);
    }
}
