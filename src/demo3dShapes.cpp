#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <imgui_impl_opengl3.h>

#include "app.hpp"
#include "appContext.hpp"
#include "demo.hpp"
#include "demoManager.hpp"
#include "demo3dShapes.hpp"
#include "shapes/shapes.hpp"

Demo3dShapes::Demo3dShapes()
{
    m_ShapeNames = {"Cube", "Cylinder", "Sphere", "Torus"};
    m_SelectedShape = 0;

    m_ClearColor = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    m_Color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);

    m_ShapePos = glm::vec2(0.0f, 0.0f);
    m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);

    m_Polygons = {
        prismCreate(0.5f, 0.5f, 0.7f, 4, 1, false, 2),      // Cube
        prismCreate(0.5f, 0.5f, 1.0f, 100, 1, false, 2),    // Cylinder
        sphereCreate(0.5, 36, 18, false, 2),                // Sphere
        torusCreate(0.5f, 0.25f, 36, 18, false, 2)          // Torus
    };

    m_VertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "uniform mat4 model;\n"
                                     "uniform mat4 view;\n"
                                     "uniform mat4 projection;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                     "}\0";

    m_FragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 ourColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = ourColor;\n"
                                       "}\n\0";
}

Demo3dShapes::~Demo3dShapes() {}

void Demo3dShapes::processEvents() {}

void Demo3dShapes::initializeGraphics()
{
    // vertex shader
    m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_VertexShader, 1, &m_VertexShaderSource, NULL);
    glCompileShader(m_VertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(m_VertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_FragmentShader, 1, &m_FragmentShaderSource, NULL);
    glCompileShader(m_FragmentShader);
    // check for shader compile errors
    glGetShaderiv(m_FragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(m_FragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, m_VertexShader);
    glAttachShader(m_ShaderProgram, m_FragmentShader);
    glLinkProgram(m_ShaderProgram);
    // check for linking errors
    glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(m_VertexShader);
    glDeleteShader(m_FragmentShader);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Polygons[m_SelectedShape].vertices.size() * sizeof(float), m_Polygons[m_SelectedShape].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[m_SelectedShape].indices.size() * sizeof(float), m_Polygons[m_SelectedShape].indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glViewport(400, 0, context::windowWidth - 400, context::windowHeight);
    glEnable(GL_DEPTH_TEST);
}

void Demo3dShapes::renderGraphics()
{
    glClearColor(m_ClearColor.x * m_ClearColor.w, m_ClearColor.y * m_ClearColor.w, m_ClearColor.z * m_ClearColor.w, m_ClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_ShaderProgram);
    int vertexColorLocation = glGetUniformLocation(m_ShaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, m_Color.x, m_Color.y, m_Color.z, m_Color.w);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_ShapePos.x, m_ShapePos.y, 0.0));
    if (m_autoRotate)
    {
        model = glm::rotate(model, ((float)SDL_GetTicks() / 500) * glm::radians(m_autoRotSpeed), glm::vec3(0.5f, 1.0f, 0.5f));
    }
    else
    {
        model = glm::rotate(model, glm::radians(m_ShapeRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_ShapeRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_ShapeRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 1520.0f / 1080.0f, 0.1f, 100.0f);

    int modelLoc = glGetUniformLocation(m_ShaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    int viewLoc = glGetUniformLocation(m_ShaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    int projectionLoc = glGetUniformLocation(m_ShaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Update selected shape
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Polygons[m_SelectedShape].vertices.size() * sizeof(float), m_Polygons[m_SelectedShape].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[m_SelectedShape].indices.size() * sizeof(unsigned int), m_Polygons[m_SelectedShape].indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_VAO);

    glDrawElements(GL_TRIANGLES, (unsigned int)m_Polygons[m_SelectedShape].indices.size(), GL_UNSIGNED_INT, 0);

}

void Demo3dShapes::renderInterface()
{
    App::startImGuiFrame();

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
            ImGuiColorEditFlags colorflags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex;
            ImGui::ColorPicker4("Shape Color", (float *)&m_Color, flags);

            const char *comboPreviewValue = m_ShapeNames[m_SelectedShape];
            static ImGuiComboFlags flags = 0;

            ImGui::SeparatorText("Shape Selection");
            if (ImGui::BeginCombo(" ", comboPreviewValue, flags))
            {
                for (int n = 0; n < m_ShapeNames.size(); n++)
                {
                    const bool isSelected = (m_SelectedShape == n);
                    if (ImGui::Selectable(m_ShapeNames[n], isSelected))
                        m_SelectedShape = n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::SeparatorText("Position");
            ImGui::SliderFloat("x position", &m_ShapePos.x, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("y position", &m_ShapePos.y, -1.0f, 1.0f, "%.3f");

            ImGui::SeparatorText("Rotation");
            ImGui::Checkbox("Auto Rotate", &m_autoRotate);
            ImGui::SliderFloat("Rotation speed", &m_autoRotSpeed, 1.0f, 100.0f, "%0.3f");
            if (m_autoRotate)
            {
                ImGui::BeginDisabled(true);
            }

            ImGui::SliderFloat("x rotation", &m_ShapeRot.x, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("y rotation", &m_ShapeRot.y, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("z rotation", &m_ShapeRot.z, 0.0f, 360.0f, "%.3f");

            if (m_autoRotate)
            {
                ImGui::EndDisabled();
            }

            if (ImGui::Button("Reset"))
            {
                m_ShapePos = glm::vec2(0.0f, 0.0f);
                m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Raycast"))
        {
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::m_DemoRaycast);
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    App::endImGuiFrame();
}

void Demo3dShapes::deallocateGraphicsData()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteProgram(m_ShaderProgram);
}

void Demo3dShapes::startNextDemo()
{
    glDisable(GL_DEPTH_TEST);
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
