#include <iostream>

#include <GL/glew.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "app.hpp"
#include "appContext.hpp"
#include "demo.hpp"
#include "demoManager.hpp"
#include "demo3dShapes.hpp"

Demo3dShapes::Demo3dShapes()
{
    m_ClearColor = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
}

Demo3dShapes::~Demo3dShapes() {}

void Demo3dShapes::processEvents() {}

void Demo3dShapes::initializeGraphics() {}

void Demo3dShapes::renderGraphics()
{
    glViewport(400, 0, context::WINDOW_WIDTH - 400, context::WINDOW_HEIGHT);
    glClearColor(m_ClearColor.x * m_ClearColor.w, m_ClearColor.y * m_ClearColor.w, m_ClearColor.z * m_ClearColor.w, m_ClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Demo3dShapes::renderInterface()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    const ImGuiViewport *mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x, mainViewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(400, 1080));
    ImGui::Begin("DemosAndParameters", nullptr, flags);
    ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Demos", tabBarFlags))
    {
        static bool isTabActive = false;
        if (ImGui::BeginTabItem("2D Shapes"))
        {
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::m_Demo2dShapes);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("3D Shapes"))
        {
            ImGui::Text("This is the 3D Shapes tab!\nblah blah blah blah blah");
            ImGui::SeparatorText("Parameters");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::Render();
}

void Demo3dShapes::deallocateGraphicsData() {}

void Demo3dShapes::startNextDemo()
{
    DemoManager::triggerNext();
}

void Demo3dShapes::run()
{
    initializeGraphics();

    while (!DemoManager::demoShouldEnd())
    {
        App::processEvents();

        renderInterface();
        renderGraphics();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(context::window);
    }

    deallocateGraphicsData();
    startNextDemo();
}
