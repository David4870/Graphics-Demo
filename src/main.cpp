#include <iostream>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "app.hpp"
#include "versionUtils.hpp"

int main(int argc, char* argv[])
{
    getSDLVersion();

    App app;
    app.run();

    return 0;
}