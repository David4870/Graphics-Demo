#pragma once

#include <imgui.h>

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
    ImVec4 m_ClearColor;
};
