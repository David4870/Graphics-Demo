#pragma once

#include <SDL3/SDL.h>

class Game
{
public:
    Game();
    ~Game();
    void run();

private:
    void initialize();
    void processEvents();
    void quit();

private:
    SDL_GLContext glContext;
};