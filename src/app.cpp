
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "appContext.hpp"
#include "app.hpp"


std::vector<float> vertices;
std::vector<int> indices;

App::App()
{
    initialize();
}

App::~App()
{
    quit();
}

void App::initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "Error: SDL_Init(): " << SDL_GetError() << std::endl; 
    }

    context::window = SDL_CreateWindow("Graphics-Demo", context::WINDOW_WIDTH, context::WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (context::window == nullptr) {
        std::cout << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl; 
    }

    context::glContext = SDL_GL_CreateContext(context::window);
    if (context::glContext == nullptr)
    {
        std::cout << "Error: SDL_GL_CreateContext():" << SDL_GetError() << std::endl; 
    }

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error: glewGetErrorString(): " << SDL_GetError() << std::endl; 
    }
}

void App::processEvents()
{
    using namespace context;
    
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE))
            running = false;
    }
}

std::pair<std::vector<float>, std::vector<int>> regularPolygon(float n, float a, float b, float r)
{
    std::vector<float> vertices;
    std::vector<int> indices;
    float theta = M_PI/2;
    float dTheta = 2*M_PI/n;

    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    for(int i = 0; i < n; i++)
    {
        theta += dTheta;
        vertices.push_back(a + r*cos(theta));
        vertices.push_back(b + r*sin(theta));
        vertices.push_back(0.0f);
    }

    for(int i = 0; i < n; i++)
    {
        indices.push_back(0);
        indices.push_back(i+1);
        indices.push_back(i+2);
    }
    indices[indices.size()-1] = 1;

    return std::make_pair(vertices, indices);
}

void App::run()
{
    using namespace context;

    const char *glsl_version = "#version 330";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 ourColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = ourColor;\n"
                                       "}\n\0";

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<float> tmpVertices;
    std::vector<int> tmpIndices;

    tie(tmpVertices, tmpIndices) = regularPolygon(6.0f, 0.0f, 0.0f, 0.75f);

    float vertices[tmpVertices.size()];
    int indices[tmpIndices.size()];

    copy(tmpVertices.begin(), tmpVertices.end(), vertices);
    copy(tmpIndices.begin(), tmpIndices.end(), indices);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);

    bool running = true;
    SDL_Event event;

    while (context::running)
    {
        processEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        {
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

            // We specify a default position/size in case there's no data in the .ini file.
            // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
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
                    ImGui::ColorPicker4("Shape Color", (float*)&color, flags);

                    // Using the generic BeginCombo() API, you have full control over how to display the combo contents.
                    // (your selection data could be an index, a pointer to the object, an id for the object, a flag intrusively
                    // stored in the object itself, etc.)
                    const char* items[] = { "Triangle", "Square", "Pentagon", "Hexagon", "Octagon", "Circle" };
                    static int item_selected_idx = 0; // Here we store our selection data as an index.

                    // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
                    const char* combo_preview_value = items[item_selected_idx];
                    static ImGuiComboFlags flags = 0;

                    if (ImGui::BeginCombo("Shape Selection", combo_preview_value, flags))
                    {
                        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                        {
                            const bool is_selected = (item_selected_idx == n);
                            if (ImGui::Selectable(items[n], is_selected))
                                item_selected_idx = n;

                            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }   

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("3D Shapes"))
                {
                    ImGui::Text("This is the 3D Shapes tab!\nblah blah blah blah blah");
                    ImGui::SeparatorText("Parameters");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Ray Casting"))
                {
                    ImGui::Text("This is the Ray Casting tab!\nblah blah blah blah blah");
                    ImGui::SeparatorText("Parameters");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Demo 4"))
                {
                    ImGui::Text("This is the Demo 4 tab!\nblah blah blah blah blah");
                    ImGui::SeparatorText("Parameters");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }

        // UI End

        // Rendering
        ImGui::Render();
        /// glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glViewport(400, 0, 1520, 1080);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, color.x, color.y, color.z, color.w);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, tmpIndices.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void App::quit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(context::glContext);
    SDL_DestroyWindow(context::window);
    SDL_Quit();
}