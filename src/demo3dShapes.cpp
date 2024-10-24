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
#include "shader.hpp"

Demo3dShapes::Demo3dShapes()
{
    m_ShapeNames = { "Cube", "Tetrahedron", "Prism", "Cylinder", "Sphere", "High-Res Sphere", "Torus", "High-Res Torus" };
    m_SelectedShape = 0;
    m_autoRotate = false;
    m_autoRotSpeed = 50;
    m_tmpRotSpeed = 50;
    m_Wireframe = false;
    m_Multicolor = false;
    m_lighting = true;
    m_specularOn = true;

    m_ClearColor = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    m_Color = ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f);

    m_ShapePos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);
    m_lightPos = glm::vec3(0.0f, 1.0f, 1.0f);

    m_Polygons = {
        prismCreate(0.5f, 0.5f, 0.7f, 4, 1, false, 2),      // Cube
        coneCreate(0.5f, 0.5f, 3, 1, false, 2),             // Tetrahedron
        prismCreate(0.5f, 0.5f, 1.0f, 3, 1, false, 2),      // Prism
        prismCreate(0.5f, 0.5f, 1.0f, 100, 1, false, 2),    // Cylinder
        sphereCreate(0.5f, 36, 18, false, 2),               // Sphere
        sphereCreate(0.5f, 200, 200, false, 2),             // High-Res Sphere
        torusCreate(0.5f, 0.25f, 36, 18, false, 2),         // Torus
        torusCreate(0.5f, 0.25f, 200, 200, false, 2)        // High-Res Torus
    };

    m_VertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 aNormal;\n"
                                     "uniform mat4 model;\n"
                                     "uniform mat4 view;\n"
                                     "uniform mat4 projection;\n"
                                     "out vec3 FragPos;\n"
                                     "out vec3 Normal;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
                                     "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
                                     "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
                                     "}\0";

    m_FragmentShaderSource = "#version 330 core\n"
                             "in vec3 FragPos;\n"
                             "in vec3 Normal;\n"
                             "out vec4 FragColor;\n"
                             "uniform vec3 ourColor;\n"
                             "uniform vec3 lightColor;\n"
                             "uniform vec3 lightPos;\n"
                             "uniform vec3 viewPos;\n"
                             "uniform vec2 resolution;\n"
                             "uniform float time;\n"
                             "uniform bool multicolor;\n"
                             "uniform bool lighting;\n"
                             "uniform bool specularOn;\n"
                             "void main()\n"
                             "{\n"
                             "   vec3 norm = normalize(Normal);\n"
                             "   vec3 lightDir = normalize(lightPos - FragPos);\n"
                             "   float diff = max(dot(norm, lightDir), 0.0);\n"
                             "   vec3 diffuse = diff * lightColor;\n"
                             "   float ambientStrength = 0.1;\n"
                             "   vec3 ambient = ambientStrength * lightColor;\n"
                             "   vec3 specular;\n"
                             "   if (specularOn)\n"
                             "   {\n"
                             "      float specularStrength = 0.5;\n"
                             "      vec3 viewDir = normalize(viewPos - FragPos);\n"
                             "      vec3 reflectDir = reflect(lightDir, norm);\n"
                             "      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
                             "      specular = specularStrength * spec * lightColor;\n"
                             "   }\n"
                             "   else\n"
                             "   {\n"
                             "      specular = vec3(0.0, 0.0, 0.0);\n"
                             "   }\n"
                             "   vec3 result = (ambient + diffuse + specular) * ourColor;\n"
                             "   vec2 uv = gl_FragCoord.xy / resolution;\n"
                             "   vec3 col = 0.5 + 0.5 * cos(time * 2.0 + uv.xyx * 5.0 + vec3(0.0, 2.0, 4.0));\n"
                             "   if (lighting)\n"
                             "   {\n"
                             "      if (multicolor)\n"
                             "      {\n"
                             "         FragColor = vec4((ambient + diffuse + specular) * col, 1.0);\n"
                             "      }\n"
                             "      else\n"
                             "      {\n"
                             "         FragColor = vec4(result, 1.0);\n"
                             "      }\n"
                             "   }\n"
                             "   else\n"
                             "   {\n"
                             "      if (multicolor)\n"
                             "      {\n"
                             "         FragColor = vec4(col, 1.0);\n"
                             "      }\n"
                             "      else\n"
                             "      {\n"
                             "         FragColor = vec4(ourColor, 1.0);\n"
                             "      }\n"
                             "   }\n"
                             "}\0";

    m_FragmentShaderLightSource = "#version 330 core\n"
                                  "out vec4 FragColor;\n"
                                  "void main()\n"
                                  "{\n"
                                  "FragColor = vec4(1.0);\n"
                                  "}\0";
}

Demo3dShapes::~Demo3dShapes() {}

void Demo3dShapes::processEvents() {}

void Demo3dShapes::initializeGraphics()
{
    m_VertexShader = createVertexShader(m_VertexShaderSource);
    m_FragmentShader = createFragmentShader(m_FragmentShaderSource);
    m_FragmentShaderLight = createFragmentShader(m_FragmentShaderLightSource);

    m_ShaderProgram = createShaderProgram(m_VertexShader, m_FragmentShader);
    m_ShaderProgramLight = createShaderProgram(m_VertexShader, m_FragmentShaderLight);

    glDeleteShader(m_VertexShader);
    glDeleteShader(m_FragmentShader);
    glDeleteShader(m_FragmentShaderLight);

    glGenVertexArrays(1, &m_VAO);
    glGenVertexArrays(1, &m_lightVAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_lightVBO);
    glGenBuffers(1, &m_EBO);
    glGenBuffers(1, &m_lightEBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Polygons[m_SelectedShape].vertices.size() * sizeof(float), m_Polygons[m_SelectedShape].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[m_SelectedShape].indices.size() * sizeof(float), m_Polygons[m_SelectedShape].indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(m_lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lightVBO);
    glBufferData(GL_ARRAY_BUFFER, m_Polygons[4].vertices.size() * sizeof(float), m_Polygons[4].vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[4].indices.size() * sizeof(float), m_Polygons[4].indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
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

    if (m_Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(m_ShapePos.x, m_ShapePos.y, m_ShapePos.z));
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

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), ((float)context::windowWidth - 400.0f) / (float)context::windowHeight, 0.1f, 100.0f);

    glUseProgram(m_ShaderProgram);
    int vertexColorLocation = glGetUniformLocation(m_ShaderProgram, "ourColor");
    int lightColorLocation = glGetUniformLocation(m_ShaderProgram, "lightColor");
    int lightPosLocation= glGetUniformLocation(m_ShaderProgram, "lightPos");
    int timeLocation = glGetUniformLocation(m_ShaderProgram, "time");
    int resolutionLocation = glGetUniformLocation(m_ShaderProgram, "resolution");
    int colorRandomLocation = glGetUniformLocation(m_ShaderProgram, "multicolor");
    int lightingBoolLocation = glGetUniformLocation(m_ShaderProgram, "lighting");
    int specularBoolLocation = glGetUniformLocation(m_ShaderProgram, "specularOn");
    int modelLoc = glGetUniformLocation(m_ShaderProgram, "model");
    int viewLoc = glGetUniformLocation(m_ShaderProgram, "view");
    int projectionLoc = glGetUniformLocation(m_ShaderProgram, "projection");

    float time = SDL_GetTicks() / 1000.0f;
    glUniform3f(vertexColorLocation, m_Color.x, m_Color.y, m_Color.z);
    glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightPosLocation, m_lightPos.x, m_lightPos.y, m_lightPos.z);
    glUniform1f(timeLocation, time);
    glUniform2f(resolutionLocation, (float)context::windowWidth, (float)context::windowHeight);
    glUniform1i(colorRandomLocation, m_Multicolor);
    glUniform1i(lightingBoolLocation, m_lighting);
    glUniform1i(specularBoolLocation, m_specularOn);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Update selected shape
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Polygons[m_SelectedShape].vertices.size() * sizeof(float), m_Polygons[m_SelectedShape].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Polygons[m_SelectedShape].indices.size() * sizeof(unsigned int), m_Polygons[m_SelectedShape].indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, (unsigned int)m_Polygons[m_SelectedShape].indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    if (m_lighting)
    {
        glUseProgram(m_ShaderProgramLight);
        glBindVertexArray(m_lightVAO);

        model = glm::mat4(1.0f);
        model = glm::translate(model, m_lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        view = glm::mat4(1.0f);
        // note that we're translating the scene in the reverse direction of where we want to move
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), ((float)context::windowWidth - 400.0f) / (float)context::windowHeight, 0.1f, 100.0f);

        int modelLocLight = glGetUniformLocation(m_ShaderProgramLight, "model");
        int viewLocLight = glGetUniformLocation(m_ShaderProgramLight, "view");
        int projectionLocLight = glGetUniformLocation(m_ShaderProgramLight, "projection");

        glUniformMatrix4fv(modelLocLight, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocLight, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocLight, 1, GL_FALSE, glm::value_ptr(projection));

        glDrawElements(GL_TRIANGLES, (unsigned int)m_Polygons[4].indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Demo3dShapes::helpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void Demo3dShapes::renderInterface()
{
    App::startImGuiFrame();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    const ImGuiViewport *mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x, mainViewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(400, context::windowHeight));
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
            ImGui::Spacing();
            ImGui::SeparatorText("Color");
            ImGuiColorEditFlags colorflags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex;
            ImGui::ColorPicker4("Shape Color", (float *)&m_Color, flags);
            ImGui::Checkbox("Multicolor", &m_Multicolor);
            ImGui::SetItemTooltip(
                "Enable and disable multicolor mode.\n"
                "Cycles through all the RGB colors.");
            ImGui::Spacing();

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
            ImGui::Spacing();

            ImGui::SeparatorText("Position");
            ImGui::SetItemTooltip("Adjust the x, y and z values of the shape to changing its position.");
            ImGui::SliderFloat("x position", &m_ShapePos.x, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("y position", &m_ShapePos.y, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("z position", &m_ShapePos.z, -1.0f, 1.0f, "%.3f");
            ImGui::Spacing();

            ImGui::SeparatorText("Rotation");
            ImGui::SetItemTooltip("Adjust the x, y and z rotation of the shape to changing its rotation.");
            ImGui::SliderFloat("##rotSpeedSlider", &m_tmpRotSpeed, -100.0f, 100.0f, "%0.3f");
            if(!ImGui::IsItemActive())
            {
                m_autoRotSpeed = m_tmpRotSpeed;
            }
            ImGui::SameLine();
            ImGui::Checkbox("Auto Rotate", &m_autoRotate);
            ImGui::SetItemTooltip("Enable and disable auto rotate. Adjust rotation speed with the slider.");

            if (m_autoRotate)
            {
                ImGui::BeginDisabled(true);
            }

            ImGui::SliderFloat("x rotation", &m_ShapeRot.x, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("y rotation", &m_ShapeRot.y, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("z rotation", &m_ShapeRot.z, 0.0f, 360.0f, "%.3f");
            ImGui::Spacing();

            if (m_autoRotate)
            {
                ImGui::EndDisabled();
            }

            if (!m_lighting)
            {
                ImGui::BeginDisabled(true);
            }
            ImGui::SeparatorText("Light position");
            ImGui::SetItemTooltip("Adjust the x, y and z values of the light source to changing its position.");
            ImGui::SliderFloat("x position##light", &m_lightPos.x, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("y position##light", &m_lightPos.y, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("z position##light", &m_lightPos.z, -1.0f, 1.0f, "%.3f");
            ImGui::Spacing();
            if (!m_lighting)
            {
                ImGui::EndDisabled();
            }

            ImGui::SeparatorText("Polygon mode");
            ImGui::Checkbox("Wireframe", &m_Wireframe);
            ImGui::SetItemTooltip(
                "Enable and disable Wireframe mode.\n"
                "If enabled, only the indices of the shape are drawn.");
            ImGui::Spacing();

            ImGui::SeparatorText("Lighting mode");
            ImGui::Checkbox("Lighting", &m_lighting);
            ImGui::SetItemTooltip("Enable and disable the lighting altogether.");
            ImGui::Checkbox("Specular reflections", &m_specularOn);
            ImGui::SetItemTooltip("Enable and disable specular reflections (highlights).");
            ImGui::Spacing();

            ImGui::Separator();
            if (ImGui::Button("Reset"))
            {
                resetParameters();
            }
            ImGui::SameLine();
            helpMarker("Reset all parameters to their default value.");

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
    glDeleteVertexArrays(1, &m_lightVAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_lightVBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_lightEBO);
    glDeleteProgram(m_ShaderProgram);
    glDeleteProgram(m_ShaderProgramLight);
}

void Demo3dShapes::resetParameters()
{
    m_ShapePos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_ShapeRot = glm::vec3(0.0f, 0.0f, 0.0f);
    m_lightPos = glm::vec3(0.0f, 1.0f, 1.0f);
    m_autoRotate = false;
    m_autoRotSpeed = 50;
    m_tmpRotSpeed = 50;
    m_Wireframe = false;
    m_Multicolor = false;
    m_lighting = true;
}

void Demo3dShapes::startNextDemo()
{
    glDisable(GL_DEPTH_TEST);
    resetParameters();
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
