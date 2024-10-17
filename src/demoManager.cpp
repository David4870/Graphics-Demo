#include "demoManager.hpp"

void DemoManager::processDemoEvents()
{
    m_CurrentDemo->processEvents();
}

void DemoManager::setNext(Demo *newDemo)
{
    m_CurrentDemo = newDemo;
    assert(m_CurrentDemo != nullptr && "Next demo was not successfully set!");
    m_IsDemoChanged = true;
}

void DemoManager::triggerNext()
{
    if (m_IsDemoChanged)
    {
        m_IsDemoChanged = false;
        context::running = true;

        m_CurrentDemo->run();
    }
}

bool DemoManager::demoShouldEnd()
{
    if (m_IsDemoChanged)
    {
        context::running = false;
    }

    return !context::running;
}

bool DemoManager::m_IsDemoChanged;
Demo *DemoManager::m_CurrentDemo = nullptr;
Demo2dShapes DemoManager::m_Demo2dShapes;
Demo3dShapes DemoManager::m_Demo3dShapes;
