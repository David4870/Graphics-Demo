#include "demoManager.hpp"

void DemoManager::setNext(Demo *newDemo)
{
    m_CurrentDemo = newDemo;
    if (m_CurrentDemo != nullptr)
    {
        m_IsDemoChanged = true;
    }
}

bool DemoManager::isChanged()
{
    return m_IsDemoChanged;
}

void DemoManager::triggerNext()
{
    if (m_IsDemoChanged)
    {
        m_IsDemoChanged = false;
        context::running = true;

        m_CurrentDemo->initializeGraphics();

        m_CurrentDemo->run();
    }
}

bool DemoManager::m_IsDemoChanged;
Demo *DemoManager::m_CurrentDemo = nullptr;
Demo2dShapes DemoManager::m_Demo2dShapes;
Demo3dShapes DemoManager::m_Demo3dShapes;