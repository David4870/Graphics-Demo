#pragma once

#include "appContext.hpp"
#include "demo.hpp"
#include "demo2dShapes.hpp"
#include "demo3dShapes.hpp"
#include "demoRaycast.hpp"

class DemoManager
{
public:
    static DemoRaycast m_DemoRaycast;
    static Demo2dShapes m_Demo2dShapes;
    static Demo3dShapes m_Demo3dShapes;

public:
    static void processDemoEvents();
    static void setNext(Demo *newDemo);
    static void triggerNext();
    static bool demoShouldEnd();

private:
    static bool m_IsDemoChanged;
    static Demo *m_CurrentDemo;
};
