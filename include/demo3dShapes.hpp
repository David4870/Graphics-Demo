#pragma once

#include <imgui.h>

#include "demo.hpp"

class Demo3dShapes : public Demo
{
public:
    Demo3dShapes();
    ~Demo3dShapes();

    void initializeGraphics() override;
    void renderGraphics() override;
    void renderInterface() override;
    void deallocateOpenGLData() override;
    void run() override;

private:
    ImVec4 m_ClearColor;
};
