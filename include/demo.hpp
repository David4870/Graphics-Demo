#pragma once

class Demo
{
public:
    virtual void initializeGraphics() = 0;
    virtual void renderGraphics() = 0;
    virtual void renderInterface() = 0;
    virtual void deallocateOpenGLData() = 0;
    virtual void run() = 0;
};
