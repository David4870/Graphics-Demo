#include <iostream>

#include <imgui_impl_sdl3.h>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "app.hpp"
#include "appContext.hpp"
#include "demoManager.hpp"
#include "demo2dShapes.hpp"
#include "initTeardown.hpp"
#include "polygon.hpp"

App::App()
{
    initialize();
}

App::~App()
{
    terminate();
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

    context::glContext = SDL_GL_CreateContext(context::window);
    if (context::glContext == nullptr)
    {
        std::cout << "Error: SDL_GL_CreateContext():" << SDL_GetError() << std::endl; 
    }

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error: glewGetErrorString(): " << SDL_GetError() << std::endl; 
    }
    initializeImGui();
}

void App::processEvents()
{
    using namespace context;
    
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
            running = false;
    }
}

void App::run()
{
    using namespace context;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    DemoManager::setNext(&DemoManager::m_Demo2dShapes);
    DemoManager::triggerNext();
}

void App::terminate()
{
    terminateImGui();

    SDL_GL_DestroyContext(context::glContext);
    SDL_DestroyWindow(context::window);
    SDL_Quit();
}