#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>

#include "demo.hpp"
#include "polygon.hpp"

class Demo2dShapes : public Demo
{
public:
    Demo2dShapes();
    ~Demo2dShapes();
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
    std::array<const char*, 6> m_ShapeNames;
    int m_SelectedShape;
    bool m_Wireframe;
    bool m_ColorRandom;

    ImVec4 m_ClearColor;
    ImVec4 m_Color;

    glm::vec2 m_ShapePos;
    glm::vec3 m_ShapeRot;
    glm::mat4 m_Trans;

    const char *m_VertexShaderSource;
    const char *m_FragmentShaderSource;
    unsigned int m_VertexShader;
    unsigned int m_FragmentShader;
    unsigned int m_ShaderProgram;

    unsigned int m_VBO, m_VAO, m_EBO;

    void randomizeColor();
    void resetParameters();
};
