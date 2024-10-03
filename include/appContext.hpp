#pragma once

#include <SDL3/SDL.h>

namespace context
{
    inline int WINDOW_WIDTH = 1920;
    inline int WINDOW_HEIGHT = 1080; 
    inline bool running = true;
    inline SDL_Event event;
    inline SDL_Window *window;
}