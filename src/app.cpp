#include <iostream>

#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "app.hpp"
#include "appContext.hpp"
#include "demoManager.hpp"
#include "demo2dShapes.hpp"
#include "initTerminate.hpp"
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
    initializeSDL();

    context::window = SDL_CreateWindow("Graphics-Demo", context::WINDOW_WIDTH, context::WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (context::window == nullptr) {
        std::cout << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl; 
    }

    context::glContext = SDL_GL_CreateContext(context::window);
    if (context::glContext == nullptr)
    {
        std::cout << "Error: SDL_GL_CreateContext():" << SDL_GetError() << std::endl; 
    }

    initializeGLEW();
    initializeImGui();
}

void App::startImGuiFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void App::endImGuiFrame()
{
    ImGui::End();
    ImGui::Render();
}

void App::processAppEvents()
{
    using namespace context;

    if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
    {
        running = false;
    }
}

void App::processEvents()
{
    using namespace context;
    
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        processAppEvents();
        DemoManager::processDemoEvents();
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
    terminateSDL();
}
