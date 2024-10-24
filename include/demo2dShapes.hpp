#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <imgui.h>

#include "demo.hpp"
#include "shapes/shapes.hpp"

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

    std::vector<unsigned int> m_Textures;
    std::array<const char*, 3> m_TextureNames;
    int m_SelectedTexture;

    bool m_Wireframe;
    bool m_Multicolor;
    bool m_TextureActive;
    bool m_TexturesLoaded;

    ImVec4 m_ClearColor;
    ImVec4 m_Color;

    glm::vec3 m_ShapePos;
    glm::vec3 m_ShapeRot;
    glm::mat4 m_Trans;

    const char *m_VertexShaderSource;
    const char *m_FragmentShaderSource;
    unsigned int m_VertexShader;
    unsigned int m_FragmentShader;
    unsigned int m_ShaderProgram;

    unsigned int m_VBO, m_VAO, m_EBO;

    void resetParameters();
};
