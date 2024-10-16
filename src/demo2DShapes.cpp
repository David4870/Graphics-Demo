#include <iostream>

#include <GL/glew.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "demoManager.hpp"
#include "demo2dShapes.hpp"
#include "appContext.hpp"

void Demo2dShapes::initializeGraphics()
{
    printf("Initialize  2D!\n");
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, polygons[item_selected_index].vertices.size() * sizeof(float), polygons[item_selected_index].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, polygons[item_selected_index].indices.size() * sizeof(unsigned int), polygons[item_selected_index].indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

void Demo2dShapes::renderGraphics()
{
    glViewport(400, 0, context::WINDOW_WIDTH - 400, context::WINDOW_HEIGHT);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our shape
    glUseProgram(shaderProgram);
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, color.x, color.y, color.z, color.w);

    // move and rotate our shape
    glm::mat4 trans = glm::mat4(1.0f);

    trans = glm::translate(trans, glm::vec3(shapePos.x, shapePos.y, 0.0)); 
    trans = glm::rotate(trans, glm::radians(shapeRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(shapeRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(shapeRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    // Update selected shape
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, polygons[item_selected_index].vertices.size() * sizeof(float), polygons[item_selected_index].vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, polygons[item_selected_index].indices.size() * sizeof(unsigned int), polygons[item_selected_index].indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(VAO); // sehexagon we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, polygons[item_selected_index].indices.size(), GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0); // no need to unbind it every time
}

void Demo2dShapes::renderInterface()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    ImGui::ShowDemoWindow();

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(400, 1080));
    ImGui::Begin("DemosAndParameters", nullptr, flags); // Create the demo selection and parameter window.
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Demos", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("2D Shapes"))
        {
            ImGui::Text("This is the 2D Shapes tab!\nblah blah blah blah blah");
            ImGui::SeparatorText("Parameters");
            ImGuiColorEditFlags colorflags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex;
            ImGui::ColorPicker4("Shape Color", (float *)&color, flags);

            // Using the generic BeginCombo() API, you have full control over how to display the combo contents.
            // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
            // stored in the object itself, etc.)

            // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
            const char *combo_preview_value = items[item_selected_index];
            static ImGuiComboFlags flags = 0;

            ImGui::SeparatorText("Shape Selection");
            if (ImGui::BeginCombo(" ", combo_preview_value, flags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_selected_index == n);
                    if (ImGui::Selectable(items[n], is_selected))
                        item_selected_index = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::SeparatorText("Position");
            ImGui::SliderFloat("x position", &shapePos.x, -1.0f, 1.0f, "%.3f");
            ImGui::SliderFloat("y position", &shapePos.y, -1.0f, 1.0f, "%.3f");

            ImGui::SeparatorText("Rotation");
            ImGui::SliderFloat("x rotation", &shapeRot.x, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("y rotation", &shapeRot.y, 0.0f, 360.0f, "%.3f");
            ImGui::SliderFloat("z rotation", &shapeRot.z, 0.0f, 360.0f, "%.3f");

            if(ImGui::Button("Reset"))
            {
                shapePos = glm::vec2(0.0f, 0.0f);
                shapeRot = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            ImGui::EndTabItem();
        }
        static bool isTabActive = false;
        if (ImGui::BeginTabItem("3D Shapes"))
        {
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::demo3dShapes);
            }
            ImGui::Text("This is the 3D Shapes tab!\nblah blah blah blah blah");
            ImGui::SeparatorText("Parameters");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::Render();
}

void Demo2dShapes::deallocateOpenGLData()
{
    // de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    printf("Destroy  2D!\n");
    DemoManager::triggerNext();
}

void Demo2dShapes::run()
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