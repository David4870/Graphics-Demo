#pragma once

class Demo
{
public:
    virtual void processEvents() = 0;
    virtual void run() = 0;

protected:
    virtual void initializeGraphics() = 0;
    virtual void renderGraphics() = 0;
    virtual void renderInterface() = 0;
    virtual void deallocateGraphicsData() = 0;
    virtual void startNextDemo() = 0;
};
