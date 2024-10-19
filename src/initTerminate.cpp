#include "initTerminate.hpp"

#include <GL/glew.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

void initializeGLEW()
{
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error: glewGetErrorString(): " << SDL_GetError() << std::endl;
    }
}

void initializeSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Error: SDL_Init(): " << SDL_GetError() << std::endl;
    }
}

void terminateSDL()
{
    SDL_GL_DestroyContext(context::glContext);
    SDL_DestroyWindow(context::window);
    SDL_Quit();
}

void initializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(context::window, context::glContext);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void terminateImGui()
{
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}
