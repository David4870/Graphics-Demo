#include <iostream>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "game.hpp"
#include "versionUtils.hpp"

int main(int argc, char* argv[])
{
    getSDLVersion();

    Game game;
    game.run();

    return 0;
}