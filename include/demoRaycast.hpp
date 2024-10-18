#pragma once

#include <imgui.h>

#include "app.hpp"
#include "demo.hpp"

class DemoRaycast : public Demo
{
public:
    ImVec4 m_ClearColor;
    ImVec4 m_SouthNorthColor;
    ImVec4 m_EastWestColor;

    static const int m_MapWidth = 10;
    static const int m_MapHeight = 10;

    std::vector<const char*> m_Items;
    int m_MapSelectedIndex;

    int m_Maps[3][m_MapWidth][m_MapHeight];

    float m_PosX = 3.0;
    float m_PosY = 3.0;
    float m_DirX = -1.0;
    float m_DirY = 0.0;
    float m_PlaneX = 0.0;
    float m_PlaneY = 0.66;

    float m_MoveSpeed = 30.0f; 
    float m_RotSpeed = 2.0f;  

    const char *m_VertexShaderSource = "#version 330 core\n"
                                     "layout(location = 0) in vec2 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                     "}\n";

    const char *m_FragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 wallColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = wallColor;\n"
                                       "}\n";

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