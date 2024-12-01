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
#include "shapes/shapes.hpp"

App::App()
{
    initialize();
}

App::~App()
{
    terminate();
}

void App::setWindowSize()
{
    float resolutionMultiplier = 1.2f;

    const SDL_DisplayMode* dm = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());
    int NATIVE_DISPLAY_RES = dm->w * dm->h;
    int FULL_HD_RES = 1920 * 1080;

    if (NATIVE_DISPLAY_RES > FULL_HD_RES)
    {
        context::windowWidth = 1920;
        context::windowHeight = 1080;
    }
    else
    {
        context::windowWidth  = 1280 * resolutionMultiplier;
        context::windowHeight = 720 * resolutionMultiplier;
    }
}

void App::initialize()
{
    initializeSDL();

    setWindowSize();

    context::window = SDL_CreateWindow("Graphics-Demo", context::windowWidth, context::windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (context::window == nullptr) {
        std::cout << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl;
    }

    context::glContext = SDL_GL_CreateContext(context::window);
    if (context::glContext == nullptr)
    {
        std::cout << "Error: SDL_GL_CreateContext():" << SDL_GetError() << std::endl;
    }

    context::isWindowFullscreen = false;

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

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F)
    {
        if (isWindowFullscreen == false)
        {
            isWindowFullscreen = true;
        }
        else
        {
            isWindowFullscreen = false;
        }

        SDL_SetWindowFullscreen(window, isWindowFullscreen);
    }

    if (event.window.type == SDL_EVENT_WINDOW_RESIZED)
    {
        windowWidth = event.window.data1;
        windowHeight = event.window.data2;
    }
}

void App::processEvents()
{
    while (SDL_PollEvent(&context::event))
    {
        ImGui_ImplSDL3_ProcessEvent(&context::event);
        processAppEvents();
        DemoManager::processDemoEvents();
    }
}

void App::run()
{
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
