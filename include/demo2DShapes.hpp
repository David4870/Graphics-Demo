#pragma once

#include "app.hpp"
#include "polygon.hpp"

class Demo2DShapes
{
public:
    const char *glsl_version = "#version 330";
    const char *items[6] = {"Triangle", "Rhombus", "Pentagon", "Hexagon", "Octagon", "Circle"};
    int item_selected_index = 0;

    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);
    ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);

    Polygon polygons[6] = {
        polygonCreate(3.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(4.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(5.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(6.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(8.0f, 0.0f, 0.0f, 0.75f),
        polygonCreate(100.0f, 0.0f, 0.0f, 0.75f),
    };

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

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
    unsigned int VBO, VAO, EBO;

    Demo2DShapes() {}
    ~Demo2DShapes() {}
    void initializeGraphics();
    void renderGraphics(); 
    void initializeInterface();
    void renderInterface();
    void deallocateOpenGLData();
    void run(); 
};