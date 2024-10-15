#pragma once

#include <imgui.h>

#include "app.hpp"
#include "demo.hpp"
#include "polygon.hpp"

class Demo3dShapes : public Demo
{
public:
    ImVec4 clear_color = ImVec4(20 / 255.0f, 20 / 255.0f, 20 / 255.0f, 1.00f);

    Demo3dShapes() {}
    ~Demo3dShapes() {}

    void initializeGraphics() override;
    void renderGraphics() override;
    void renderInterface() override;
    void deallocateOpenGLData() override;
    void run() override;
};
