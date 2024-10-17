#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_impl_opengl3.h>

#include "app.hpp"
#include "appContext.hpp"
#include "demoManager.hpp"
#include "demo2dShapes.hpp"
#include "polygon.hpp"

Demo2dShapes::Demo2dShapes()
{
    m_Shapes = {"Triangle", "Rhombus", "Pentagon", "Hexagon", "Octagon", "Circle"};
    m_SelectedShape = 0;

    m_ClearColor = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    m_Color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);

    m_ShapePos = glm::vec2(0.0f, 0.0f);
    m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);

    m_Polygons = {
        polygonCreate(3.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(4.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(5.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(6.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(8.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(100.0f, 0.0f, 0.0f, 0.75f),
    };

    m_VertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "uniform mat4 transform;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = transform * vec4(aPos, 1.0);\n"
                                     "}\0";

    m_FragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 ourColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = ourColor;\n"
                                       "}\n\0";
}

Demo2dShapes::~Demo2dShapes() {}

void Demo2dShapes::processEvents() {}

void Demo2dShapes::initializeGraphics()
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[m_SelectedShape].indices.size() * sizeof(unsigned int), m_Polygons[m_SelectedShape].indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glViewport(400, 0, context::WINDOW_WIDTH - 400, context::WINDOW_HEIGHT);
}

void Demo2dShapes::renderGraphics()
{
    glClearColor(m_ClearColor.x * m_ClearColor.w, m_ClearColor.y * m_ClearColor.w, m_ClearColor.z * m_ClearColor.w, m_ClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our shape
    glUseProgram(m_ShaderProgram);
    int vertexColorLocation = glGetUniformLocation(m_ShaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, m_Color.x, m_Color.y, m_Color.z, m_Color.w);

    // move and rotate our shape
    glm::mat4 m_Trans = glm::mat4(1.0f);

    m_Trans = glm::translate(m_Trans, glm::vec3(m_ShapePos.x, m_ShapePos.y, 0.0)); 
    m_Trans = glm::rotate(m_Trans, glm::radians(m_ShapeRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    m_Trans = glm::rotate(m_Trans, glm::radians(m_ShapeRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m_Trans = glm::rotate(m_Trans, glm::radians(m_ShapeRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    unsigned int transformLoc = glGetUniformLocation(m_ShaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(m_Trans));

    // Update selected shape
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Polygons[m_SelectedShape].vertices.size() * sizeof(float), m_Polygons[m_SelectedShape].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[m_SelectedShape].indices.size() * sizeof(unsigned int), m_Polygons[m_SelectedShape].indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_VAO); // sehexagon we only have a single m_VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, m_Polygons[m_SelectedShape].indices.size(), GL_UNSIGNED_INT, 0);
}

void Demo2dShapes::renderInterface()
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
        if (ImGui::BeginTabItem("2D Shapes"))
        {
            ImGui::Text("This is the 2D Shapes tab!\nblah blah blah blah blah");
            ImGui::SeparatorText("Parameters");
            ImGuiColorEditFlags colorflags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex;
            ImGui::ColorPicker4("Shape Color", (float *)&m_Color, flags);

            const char *comboPreviewValue = m_Shapes[m_SelectedShape];
            static ImGuiComboFlags flags = 0;

            ImGui::SeparatorText("Shape Selection");
            if (ImGui::BeginCombo(" ", comboPreviewValue, flags))
            {
                for (int n = 0; n < m_Shapes.size(); n++)
                {
                    const bool isSelected = (m_SelectedShape == n);
                    if (ImGui::Selectable(m_Shapes[n], isSelected))
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
            ImGui::SliderFloat("x rotation", &m_ShapeRot.x, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("y rotation", &m_ShapeRot.y, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("z rotation", &m_ShapeRot.z, 0.0f, 360.0f, "%.3f");

            if (ImGui::Button("Reset"))
            {
                m_ShapePos = glm::vec2(0.0f, 0.0f);
                m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            ImGui::EndTabItem();
        }
        static bool isTabActive = false;
        if (ImGui::BeginTabItem("3D Shapes"))
        {
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::m_Demo3dShapes);
            }
            ImGui::Text("This is the 3D Shapes tab!\nblah blah blah blah blah");
            ImGui::SeparatorText("Parameters");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    App::endImGuiFrame();
}

void Demo2dShapes::deallocateGraphicsData()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteProgram(m_ShaderProgram);
}

void Demo2dShapes::startNextDemo()
{
    DemoManager::triggerNext();
}

void Demo2dShapes::run()
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
