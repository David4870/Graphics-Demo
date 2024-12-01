#pragma once

#include <SDL3/SDL.h>

namespace context
{
    inline int windowWidth = 1920;
    inline int windowHeight = 1080;
    inline bool running;
    inline bool isWindowFullscreen;
    inline SDL_Event event;
    inline SDL_Window *window;
    inline SDL_GLContext glContext;
}
