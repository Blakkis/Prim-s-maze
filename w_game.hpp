#pragma once

#include "SDL.h"

class W_Game
{
    public:
        W_Game(const char *title, int width, int height);
        ~W_Game(void);

        void w_gameloop(void);
        void w_clear(uint32_t color);
        void w_flip(void);
        void w_drawRect(SDL_Texture *target, int x1, int y1, int x2, int y2, uint32_t color, bool _fill = false);
        void w_drawLine(SDL_Texture *target, int x1, int y1, int x2, int y2, uint32_t color);


    private:
        SDL_Window   *wWindow;
        SDL_Renderer *wRenderer;
        int wWidth;
        int wHeight;
};
