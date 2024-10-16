#include <iostream>

#include <GL/glew.h>
#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "demoManager.hpp"
#include "demoRaycast.hpp"
#include "appContext.hpp"

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
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void DemoRaycast::processInput(const Uint8* keys, float deltaTime)
{
    float moveStep = moveSpeed * deltaTime;

    if (keys[SDL_SCANCODE_W])
    {
        if (worldMap[int(posX + dirX * moveStep)][int(posY)] == 0) posX += dirX * moveStep;
        if (worldMap[int(posX)][int(posY + dirY * moveStep)] == 0) posY += dirY * moveStep;
    }
    if (keys[SDL_SCANCODE_S])
    {
        if (worldMap[int(posX - dirX * moveStep)][int(posY)] == 0) posX -= dirX * moveStep;
        if (worldMap[int(posX)][int(posY - dirY * moveStep)] == 0) posY -= dirY * moveStep;
    }
    if (keys[SDL_SCANCODE_A])
    {
        if (worldMap[int(posX - planeX * moveStep)][int(posY)] == 0) posX -= planeX * moveStep;
        if (worldMap[int(posX)][int(posY - planeY * moveStep)] == 0) posY -= planeY * moveStep;
    }
    if (keys[SDL_SCANCODE_D])
    {
        if (worldMap[int(posX + planeX * moveStep)][int(posY)] == 0) posX += planeX * moveStep;
        if (worldMap[int(posX)][int(posY + planeY * moveStep)] == 0) posY += planeY * moveStep;
    }
}

void DemoRaycast::processMouseInput(float xOffset, float deltaTime)
{
    float rotStep = rotSpeed * deltaTime * xOffset;
    float oldDirX = dirX;
    dirX = dirX * cos(-rotStep) - dirY * sin(-rotStep);
    dirY = oldDirX * sin(-rotStep) + dirY * cos(-rotStep);
    float oldPlaneX = planeX;
    planeX = planeX * cos(-rotStep) - planeY * sin(-rotStep);
    planeY = oldPlaneX * sin(-rotStep) + planeY * cos(-rotStep);
}


void DemoRaycast::renderScene(int windowWidth, int windowHeight)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    for (int x = 0; x < windowWidth; x++)
    {
        float cameraX = 2 * x / (float)windowWidth - 1;
        float rayDirX = dirX + planeX * cameraX;
        float rayDirY = dirY + planeY * cameraX;

        int mapX = (int)posX;
        int mapY = (int)posY;

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
            sideDistX = (posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
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
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
        else perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

        int lineHeight = (int)(windowHeight / perpWallDist);

        int drawStart = -lineHeight / 2 + windowHeight / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + windowHeight / 2;
        if (drawEnd >= windowHeight) drawEnd = windowHeight - 1;

        float wallColor[3];
        if (side == 1)
        {
            wallColor[0] = 0.7f;
            wallColor[1] = 0.7f;
            wallColor[2] = 0.7f;
        }
        else
        {
            wallColor[0] = 1.0f;
            wallColor[1] = 0.0f;
            wallColor[2] = 0.0f;
        }

        int colorLoc = glGetUniformLocation(shaderProgram, "wallColor");
        glUniform3fv(colorLoc, 1, wallColor);

        float vertices[] = {
            2.0f * x / windowWidth - 1.0f, 2.0f * drawStart / windowHeight - 1.0f,
            2.0f * x / windowWidth - 1.0f, 2.0f * drawEnd / windowHeight - 1.0f
        };

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }
}

void DemoRaycast::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DemoRaycast::initializeGraphics()
{
    printf("Initialize  Raycast!\n");
    createShaderProgram();
    setupBuffers();
}

void DemoRaycast::renderGraphics()
{
    glViewport(400, 0, context::WINDOW_WIDTH - 400, context::WINDOW_HEIGHT);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    renderScene(context::WINDOW_WIDTH- 400, context::WINDOW_HEIGHT);
}

void DemoRaycast::renderInterface()
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
    static bool isTabActive = false;
    if (ImGui::BeginTabBar("Demos", tab_bar_flags))
    {
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
            if (!isTabActive)
            {
                DemoManager::setNext(&DemoManager::demo3dShapes);
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
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    ImGui::Render();
}

void DemoRaycast::deallocateOpenGLData()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    printf("Destroy Raycast!\n");
    DemoManager::triggerNext();
}

void DemoRaycast::run()
{
    Uint32 lastTime = SDL_GetTicks();
    int mouseLocked = 0;
    while (context::running)
    {
        while (SDL_PollEvent(&context::event))
        {
            ImGui_ImplSDL3_ProcessEvent(&context::event);

            if (context::event.type == SDL_EVENT_KEY_DOWN && context::event.key.key == SDLK_SPACE)
            {
                if (mouseLocked == 0)
                {
                    mouseLocked = 1;
                }
                else
                {
                    mouseLocked = 0;
                }
                SDL_SetRelativeMouseMode(mouseLocked); // Enable or disable relative mouse mode
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 10000.0f;
        lastTime = currentTime;

        App::processEvents();

        float mouseX, mouseY;
        SDL_GetRelativeMouseState(&mouseX, &mouseY);
        processMouseInput(mouseX, deltaTime);

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        processInput(keys, deltaTime);

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