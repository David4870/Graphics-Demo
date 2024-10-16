#include <iostream>

#include <GL/glew.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "demo.hpp"
#include "demoManager.hpp"
#include "demo3dShapes.hpp"
#include "appContext.hpp"

void Demo3dShapes::initializeGraphics()
{

}

void Demo3dShapes::renderGraphics()
{
    glViewport(400, 0, context::WINDOW_WIDTH - 400, context::WINDOW_HEIGHT);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Demo3dShapes::renderInterface()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(400, 1080));
    ImGui::Begin("DemosAndParameters", nullptr, flags); // Create the demo selection and parameter window.
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Demos", tab_bar_flags))
    {
        static bool isTabActive = false;
        if (ImGui::BeginTabItem("2D Shapes"))
        {
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::demo2dShapes);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("3D Shapes"))
        {
            ImGui::Text("This is the 3D Shapes tab!\nblah blah blah blah blah");
            ImGui::SeparatorText("Parameters");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Raycast"))
        {
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::demoRaycast);
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::Render();
}

void Demo3dShapes::deallocateOpenGLData()
{
    DemoManager::triggerNext();
}

void Demo3dShapes::run()
{
    while (context::running)
    {
        App::processEvents();

        renderInterface();
        renderGraphics();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(context::window);
        if (DemoManager::isChanged())
        {
            context::running = false;
        }
    }
    deallocateOpenGLData();
}
