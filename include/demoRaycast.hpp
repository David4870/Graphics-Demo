#pragma once

#include <imgui.h>

#include "app.hpp"
#include "demo.hpp"

class DemoRaycast : public Demo
{
public:

    ImVec4 clear_color = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    ImVec4 southNorthColor = ImVec4(200.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
    ImVec4 eastWestColor = ImVec4(178.0f / 255.0f, 178.0f / 255.0f, 178.0f / 255.0f, 255.0f / 255.0f);
    int WINDOW_WIDTH = 1520;
    int WINDOW_HEIGHT = 1080;

    static const int mapWidth = 10;
    static const int mapHeight = 10;

    const char *items[3] = {"Cube", "Maze", "Rooms"};
    int map_selected_index = 0;

    int maps[3][mapWidth][mapHeight] = {
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

    float posX = 3.0, posY = 3.0;
    float dirX = -1.0, dirY = 0.0;
    float planeX = 0.0, planeY = 0.66;

    float moveSpeed = 30.0f; 
    float rotSpeed = 2.0f;  

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout(location = 0) in vec2 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                     "}\n";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 wallColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = wallColor;\n"
                                       "}\n";

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    unsigned int VBO, VAO;

    DemoRaycast() {}
    ~DemoRaycast() {}

    void checkShaderCompilation(unsigned int intshader);
    void checkProgramLinking(unsigned int program);
    void createShaderProgram();
    void changeMap();
    void processInput(const Uint8* keys, float deltaTime);
    void processMouseInput(float xOffset, float deltaTime);
    void setupBuffers();
    void renderScene(int windowWidth, int windowHeight);
    void initializeGraphics() override;
    void renderGraphics() override;
    void renderInterface() override;
    void deallocateOpenGLData() override;
    void run() override;
};