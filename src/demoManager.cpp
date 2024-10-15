#include "demoManager.hpp"

void DemoManager::setNext(Demo *newDemo)
{
    currentDemo = newDemo;
    if (currentDemo != nullptr)
    {
        isDemoChanged = true;
    }
}

bool DemoManager::isChanged()
{
    return isDemoChanged;
}

void DemoManager::triggerNext()
{
    if (isDemoChanged)
    {
        isDemoChanged = false;
        context::running = true;

        currentDemo->initializeGraphics();

        currentDemo->run();
    }
}

bool DemoManager::isDemoChanged;
Demo *DemoManager::currentDemo = nullptr;
Demo2dShapes DemoManager::demo2dShapes;
Demo3dShapes DemoManager::demo3dShapes;