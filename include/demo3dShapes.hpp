#pragma once

#include <imgui.h>

#include <array>

#include "demo.hpp"

class Demo3dShapes : public Demo
{
public:
    Demo3dShapes();
    ~Demo3dShapes();
    void processEvents() override;
    void run() override;

protected:
    void initializeGraphics() override;
    void renderGraphics() override;
    void renderInterface() override;
    void deallocateGraphicsData() override;
    void startNextDemo() override;

private:
    std::vector<Polygon> m_Polygons;
    std::array<const char*, 4> m_ShapeNames;
    int m_SelectedShape;
    bool m_Wireframe;
    bool m_Multicolor;

    ImVec4 m_ClearColor;
    ImVec4 m_Color;

    glm::vec2 m_ShapePos;
    glm::vec3 m_ShapeRot;
    glm::mat4 m_Trans;

    bool m_autoRotate = false;
    float m_autoRotSpeed = 50;

    const char *m_VertexShaderSource;
    const char *m_FragmentShaderSource;
    unsigned int m_VertexShader;
    unsigned int m_FragmentShader;
    unsigned int m_ShaderProgram;

    unsigned int m_VBO, m_VAO, m_EBO;

    void resetParameters();
};
