#include <iostream>

#include <GL/glew.h>

#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "app.hpp"
#include "appContext.hpp"
#include "demoManager.hpp"
#include "demoRaycast.hpp"

DemoRaycast::DemoRaycast()
{
    m_ClearColor = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    m_SouthNorthColor = ImVec4(200.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
    m_EastWestColor = ImVec4(178.0f / 255.0f, 178.0f / 255.0f, 178.0f / 255.0f, 255.0f / 255.0f);

    m_MapNames = {"Cube", "Maze", "Rooms"};
    m_MapSelected = 0;

    loadMaps();

    m_PosX = 3.0;
    m_PosY = 3.0;
    m_DirX = -1.0;
    m_DirY = 0.0;
    m_PlaneX = 0.0;
    m_PlaneY = 0.66;

    m_MoveSpeed = 30.0f; 
    m_RotSpeed = 2.0f;  

    m_VertexShaderSource = "#version 330 core\n"
                                     "layout(location = 0) in vec2 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                     "}\n";

    m_FragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 wallColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = wallColor;\n"
                                       "}\n";
}

DemoRaycast::~DemoRaycast() {}

void DemoRaycast::loadMaps()
{
    int mapsToLoad[TOTAL_MAPS][MAP_WIDTH][MAP_HEIGHT] = {
        {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
        },
        {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 1, 0, 1, 1, 1, 0, 1},
            {1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
            {1, 0, 0, 1, 1, 1, 0, 0, 0, 1},
            {1, 0, 1, 1, 0, 0, 0, 1, 0, 1},
            {1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
            {1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
        },
        {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
            {1, 0, 0, 1, 1, 0, 1, 0, 0, 1},
            {1, 0, 0, 1, 1, 0, 1, 0, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 1, 1, 1, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
        },
    };

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 10; k++)
            {
                m_Maps[i][j][k] = mapsToLoad[i][j][k];
            }
        }
    }
}

void DemoRaycast::processEvents()
{
    using namespace context;

    static int mouseLocked = 0;

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE)
    {
        if (mouseLocked == 0)
        {
            mouseLocked = 1;
        }
        else
        {
            mouseLocked = 0;
        }
        SDL_SetRelativeMouseMode(mouseLocked);
    }
}

void DemoRaycast::checkShaderCompilation(unsigned int shader)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
    }
}

void DemoRaycast::checkProgramLinking(unsigned int program)
{
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Program Linking Error: " << infoLog << std::endl;
    }
}

void DemoRaycast::createShaderProgram()
{
    m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_VertexShader, 1, &m_VertexShaderSource, NULL);
    glCompileShader(m_VertexShader);
    checkShaderCompilation(m_VertexShader);

    m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_FragmentShader, 1, &m_FragmentShaderSource, NULL);
    glCompileShader(m_FragmentShader);
    checkShaderCompilation(m_FragmentShader);

    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, m_VertexShader);
    glAttachShader(m_ShaderProgram, m_FragmentShader);
    glLinkProgram(m_ShaderProgram);
    checkProgramLinking(m_ShaderProgram);

    glDeleteShader(m_VertexShader);
    glDeleteShader(m_FragmentShader);
}

void DemoRaycast::processInput(const Uint8* keys, float deltaTime)
{
    float moveStep = m_MoveSpeed * deltaTime;

    if (keys[SDL_SCANCODE_W])
    {
        if (m_Maps[m_MapSelected][int(m_PosX + m_DirX * moveStep)][int(m_PosY)] == 0) m_PosX += m_DirX * moveStep;
        if (m_Maps[m_MapSelected][int(m_PosX)][int(m_PosY + m_DirY * moveStep)] == 0) m_PosY += m_DirY * moveStep;
    }
    if (keys[SDL_SCANCODE_S])
    {
        if (m_Maps[m_MapSelected][int(m_PosX - m_DirX * moveStep)][int(m_PosY)] == 0) m_PosX -= m_DirX * moveStep;
        if (m_Maps[m_MapSelected][int(m_PosX)][int(m_PosY - m_DirY * moveStep)] == 0) m_PosY -= m_DirY * moveStep;
    }
    if (keys[SDL_SCANCODE_A])
    {
        if (m_Maps[m_MapSelected][int(m_PosX - m_PlaneX * moveStep)][int(m_PosY)] == 0) m_PosX -= m_PlaneX * moveStep;
        if (m_Maps[m_MapSelected][int(m_PosX)][int(m_PosY - m_PlaneY * moveStep)] == 0) m_PosY -= m_PlaneY * moveStep;
    }
    if (keys[SDL_SCANCODE_D])
    {
        if (m_Maps[m_MapSelected][int(m_PosX + m_PlaneX * moveStep)][int(m_PosY)] == 0) m_PosX += m_PlaneX * moveStep;
        if (m_Maps[m_MapSelected][int(m_PosX)][int(m_PosY + m_PlaneY * moveStep)] == 0) m_PosY += m_PlaneY * moveStep;
    }
}

void DemoRaycast::processMouseInput(float xOffset, float deltaTime)
{
    float rotStep = m_RotSpeed * deltaTime * xOffset;
    float oldDirX = m_DirX;
    m_DirX = m_DirX * cos(-rotStep) - m_DirY * sin(-rotStep);
    m_DirY = oldDirX * sin(-rotStep) + m_DirY * cos(-rotStep);
    float oldPlaneX = m_PlaneX;
    m_PlaneX = m_PlaneX * cos(-rotStep) - m_PlaneY * sin(-rotStep);
    m_PlaneY = oldPlaneX * sin(-rotStep) + m_PlaneY * cos(-rotStep);
}


void DemoRaycast::renderScene(int windowWidth, int windowHeight)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_ShaderProgram);
    static int counter = 0;
    for (int x = 0; x < windowWidth; x++)
    {
        float cameraX = 2 * x / (float)windowWidth - 1;
        float rayDirX = m_DirX + m_PlaneX * cameraX;
        float rayDirY = m_DirY + m_PlaneY * cameraX;

        int mapX = (int)m_PosX;
        int mapY = (int)m_PosY;

        float sideDistX, sideDistY;

        float deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);
        float perpWallDist;

        int stepX, stepY;
        int hit = 0;
        int side;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (m_PosX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - m_PosX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (m_PosY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - m_PosY) * deltaDistY;
        }

        while (hit == 0)
        {
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (m_Maps[m_MapSelected][mapX][mapY] > 0) hit = 1;
        }

        if (side == 0) perpWallDist = (mapX - m_PosX + (1 - stepX) / 2) / rayDirX;
        else perpWallDist = (mapY - m_PosY + (1 - stepY) / 2) / rayDirY;

        int lineHeight = (int)(windowHeight / perpWallDist);

        int drawStart = -lineHeight / 2 + windowHeight / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + windowHeight / 2;
        if (drawEnd >= windowHeight) drawEnd = windowHeight - 1;

        int colorLoc = glGetUniformLocation(m_ShaderProgram, "wallColor");

        if (side == 1)
        {
            glUniform4f(colorLoc, m_SouthNorthColor.x, m_SouthNorthColor.y, m_SouthNorthColor.z, m_SouthNorthColor.w);
        }
        else
        {
            glUniform4f(colorLoc, m_EastWestColor.x, m_EastWestColor.y, m_EastWestColor.z, m_EastWestColor.w);
        }

        float vertices[] = {
            2.0f * x / windowWidth - 1.0f, 2.0f * drawStart / windowHeight - 1.0f,
            2.0f * x / windowWidth - 1.0f, 2.0f * drawEnd / windowHeight - 1.0f
        };

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }
}

void DemoRaycast::setupBuffers()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DemoRaycast::initializeGraphics()
{
    createShaderProgram();
    setupBuffers();
    glViewport(400, 0, context::windowWidth - 400, context::windowHeight);
}

void DemoRaycast::renderGraphics()
{
    glClearColor(m_ClearColor.x * m_ClearColor.w, m_ClearColor.y * m_ClearColor.w, m_ClearColor.z * m_ClearColor.w, m_ClearColor.w);
    renderScene(context::windowWidth - 400, context::windowHeight);
}

void DemoRaycast::renderInterface()
{
    App::startImGuiFrame();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
    ImGui::SetNextWindowSize(ImVec2(400, 1080));
    ImGui::Begin("DemosAndParameters", nullptr, flags); // Create the demo selection and parameter window.
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    static bool isTabActive = false;
    if (ImGui::BeginTabBar("Demos", tab_bar_flags))
    {
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
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::m_Demo3dShapes);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Raycast"))
        {
            ImGui::Text("This is the Raycast Demo!");
            ImGui::NewLine();
            ImGui::Text("Press WASD to move around. Use your mouse to look around.");
            ImGui::Text("Press SPACE to toggle RelativeMouseMode.");

            ImGui::SeparatorText("Parameters");
            if (ImGui::CollapsingHeader("Wall Colors", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGuiColorEditFlags colorflags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex;
                ImGui::ColorPicker4("South/North color", (float *)&m_SouthNorthColor, flags);
                ImGui::ColorPicker4("East/West color", (float *)&m_EastWestColor, flags);
            }

            const char *combo_preview_value = m_MapNames[m_MapSelected];
            static ImGuiComboFlags flags = 0;
            ImGui::SeparatorText("Map Selection");
            if (ImGui::BeginCombo(" ", combo_preview_value, flags))
            {
                for (int n = 0; n < m_MapNames.size(); n++)
                {
                    const bool is_selected = (m_MapSelected == n);
                    if (ImGui::Selectable(m_MapNames[n], is_selected))
                        m_MapSelected = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    App::endImGuiFrame();
}

void DemoRaycast::deallocateGraphicsData()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_ShaderProgram);
}

void DemoRaycast::startNextDemo()
{
    DemoManager::triggerNext();
}

void DemoRaycast::run()
{
    initializeGraphics();

    Uint32 lastTime = SDL_GetTicks();
    while (!DemoManager::demoShouldEnd())
    {
        App::processEvents();

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 10000.0f;
        lastTime = currentTime;

        float mouseX, mouseY;
        SDL_GetRelativeMouseState(&mouseX, &mouseY);
        if (SDL_GetRelativeMouseMode() == true)
        {
            processMouseInput(mouseX, deltaTime);
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        processInput(keys, deltaTime);

        renderInterface();
        renderGraphics();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(context::window);
    }

    deallocateGraphicsData();
    startNextDemo();
}