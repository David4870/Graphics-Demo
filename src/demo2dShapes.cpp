#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_impl_opengl3.h>

#include "app.hpp"
#include "appContext.hpp"
#include "demoManager.hpp"
#include "demo2dShapes.hpp"
#include "shapes/shapes.hpp"


Demo2dShapes::Demo2dShapes()
{
    m_ShapeNames = {"Triangle", "Square", "Pentagon", "Hexagon", "Octagon", "Circle"};
    m_SelectedShape = 0;
    m_Wireframe = false;
    m_Multicolor = false;

    m_ClearColor = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    m_Color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);

    m_ShapePos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);

    m_Polygons = {
        polygon2dCreate(3.0f, 0.0f, 0.0f, 0.75f),
        polygon2dCreate(4.0f, 0.0f, 0.0f, 0.75f),
        polygon2dCreate(5.0f, 0.0f, 0.0f, 0.75f),
        polygon2dCreate(6.0f, 0.0f, 0.0f, 0.75f),
        polygon2dCreate(8.0f, 0.0f, 0.0f, 0.75f),
        polygon2dCreate(100.0f, 0.0f, 0.0f, 0.75f),
    };

    m_VertexShaderSource = "#version 330 core\n"
                           "layout (location = 0) in vec3 aPos;\n"
                           "uniform mat4 model;\n"
                           "uniform mat4 view;\n"
                           "uniform mat4 projection;\n"
                           "void main()\n"
                           "{\n"
                           "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                           "}\n\0";

    m_FragmentShaderSource = "#version 330 core\n"
                             "out vec4 FragColor;\n"
                             "uniform vec4 ourColor;\n"
                             "uniform vec2 resolution;\n"
                             "uniform float time;\n"
                             "uniform bool multicolor;\n"
                             "void main()\n"
                             "{\n"
                             "   vec2 uv = gl_FragCoord.xy / resolution;\n"
                             "   vec3 col = 0.5 + 0.5 * cos(time * 2.0 + uv.xyx * 5.0 + vec3(0.0, 2.0, 4.0));\n"
                             "   if (multicolor)\n"
                             "   {\n"
                             "      FragColor = vec4(col, 1.0);\n"
                             "   }\n"
                             "   else\n"
                             "   {\n"
                             "      FragColor = ourColor;\n"
                             "   }\n"
                             "}\n";
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

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glViewport(400, 0, context::windowWidth - 400, context::windowHeight);
}

void Demo2dShapes::renderGraphics()
{
    glClearColor(m_ClearColor.x * m_ClearColor.w, m_ClearColor.y * m_ClearColor.w, m_ClearColor.z * m_ClearColor.w, m_ClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUseProgram(m_ShaderProgram);
    int vertexColorLocation = glGetUniformLocation(m_ShaderProgram, "ourColor");
    int modelLocation = glGetUniformLocation(m_ShaderProgram, "model");
    int viewLocation = glGetUniformLocation(m_ShaderProgram, "view");
    int projectionLocation = glGetUniformLocation(m_ShaderProgram, "projection");
    int timeLocation = glGetUniformLocation(m_ShaderProgram, "time");
    int resolutionLocation = glGetUniformLocation(m_ShaderProgram, "resolution");
    int colorRandomLocation = glGetUniformLocation(m_ShaderProgram, "multicolor");

    // Update selected shape
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Polygons[m_SelectedShape].vertices.size() * sizeof(float), m_Polygons[m_SelectedShape].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[m_SelectedShape].indices.size() * sizeof(unsigned int), m_Polygons[m_SelectedShape].indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_ShapePos);
    model = glm::rotate(model , glm::radians(m_ShapeRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model , glm::radians(m_ShapeRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model , glm::radians(m_ShapeRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), 1520.0f / 1080.0f, 0.1f, 100.0f);

    float time = SDL_GetTicks() / 1000.0f;
    glUniform4f(vertexColorLocation, m_Color.x, m_Color.y, m_Color.z, m_Color.w);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1f(timeLocation, time);
    glUniform2f(resolutionLocation, (float)context::windowWidth, (float)context::windowHeight);
    glUniform1i(colorRandomLocation, m_Multicolor);


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
            ImGui::Checkbox("Multicolor", &m_Multicolor);

            const char *comboPreviewValue = m_ShapeNames[m_SelectedShape];
            static ImGuiComboFlags flags = 0;

            ImGui::SeparatorText("Shape selection");
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
            ImGui::SliderFloat("x rotation", &m_ShapeRot.x, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("y rotation", &m_ShapeRot.y, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("z rotation", &m_ShapeRot.z, 0.0f, 360.0f, "%.3f");
            ImGui::SeparatorText("Polygon mode");
            ImGui::Checkbox("Wireframe", &m_Wireframe);

            ImGui::Separator();
            if (ImGui::Button("Reset"))
            {
                resetParameters();
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

void Demo2dShapes::deallocateGraphicsData()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteProgram(m_ShaderProgram);
}

void Demo2dShapes::resetParameters()
{
    m_ShapePos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);
    m_Wireframe = false;
    m_Multicolor = false;
}

void Demo2dShapes::startNextDemo()
{
    resetParameters();
    DemoManager::triggerNext();
}

void Demo2dShapes::run()
{
    initializeGraphics();

    while (!DemoManager::demoShouldEnd())
    {
        App::processEvents();

        // randomizeColor();
        renderInterface();
        renderGraphics();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(context::window);
    }

    deallocateGraphicsData();
    startNextDemo();
}
