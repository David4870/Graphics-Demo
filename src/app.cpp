#include <iostream>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "appContext.hpp"
#include "app.hpp"

App::App()
{
    initialize();
}

App::~App()
{
    quit();
}

void App::initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Error: SDL_Init(): " << SDL_GetError() << std::endl; 
    }

    context::window = SDL_CreateWindow("Graphics-Demo", context::WINDOW_WIDTH, context::WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (context::window == nullptr) {
        std::cout << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl; 
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(context::window);
    if (glContext == nullptr)
    {
        std::cout << "Error: SDL_GL_CreateContext():" << SDL_GetError() << std::endl; 
    }

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error: glewGetErrorString(): " << SDL_GetError() << std::endl; 
    }
}

void App::processEvents()
{
    using namespace context;
    
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
            running = false;
    }
}

void App::run()
{
    while (context::running)
    {
        processEvents();

        glClearColor(50 / 255.0f, 50 / 255.0f, 50 / 255.0f, 255 / 255.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        SDL_GL_SwapWindow(context::window);
    }
}

void App::quit()
{
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(context::window);
    SDL_Quit();
}