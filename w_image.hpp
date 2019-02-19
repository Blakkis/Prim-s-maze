#pragma once

#include "SDL.h"
#include <string>

class W_Texture
{
    public:
        W_Texture(SDL_Renderer *renderer, const std::string &texture_path,
                  int texture_rows = 0, int texture_columns = 0);

        void render(int x, int y, bool outline = false, int row = -1, int column = -1);

    private:
        SDL_Renderer *t_renderer;
        SDL_Texture  *t_texture;
        SDL_Rect      t_texture_rect_dst;
        SDL_Rect      t_texture_rect_src;

        //
        int t_num_of_rows;
        int t_max_index_x;

        int t_num_of_columns;
        int t_max_index_y;
};
