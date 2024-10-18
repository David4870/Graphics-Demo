#pragma once

#include <imgui.h>

#include "demo.hpp"

#define TOTAL_MAPS 3
#define MAP_WIDTH 10
#define MAP_HEIGHT 10

class DemoRaycast : public Demo
{
public:
    ImVec4 m_ClearColor;
    ImVec4 m_SouthNorthColor;
    ImVec4 m_EastWestColor;

    std::vector<const char*> m_MapNames;
    int m_MapSelected;

    int m_Maps[TOTAL_MAPS][MAP_WIDTH][MAP_HEIGHT];

    float m_PosX, m_PosY;
    float m_DirX, m_DirY;
    float m_PlaneX, m_PlaneY;

    float m_MoveSpeed; 
    float m_RotSpeed;  

    const char *m_VertexShaderSource;
    const char *m_FragmentShaderSource;
    unsigned int m_VertexShader;
    unsigned int m_FragmentShader;
    unsigned int m_ShaderProgram;
    unsigned int m_VBO, m_VAO;

    DemoRaycast();
    ~DemoRaycast();

    void processEvents() override;
    void run() override;

protected:
    void initializeGraphics() override;
    void renderGraphics() override;
    void renderInterface() override;
    void deallocateGraphicsData() override;
    void startNextDemo() override;

private:
    void loadMaps();
    void checkShaderCompilation(unsigned int intshader);
    void checkProgramLinking(unsigned int program);
    void createShaderProgram();
    void processInput(const Uint8* keys, float deltaTime);
    void processMouseInput(float xOffset, float deltaTime);
    void setupBuffers();
    void renderScene(int windowWidth, int windowHeight);
};