#pragma once

#include "appContext.hpp"
#include "demo.hpp"
#include "demo2dShapes.hpp"
#include "demo3dShapes.hpp"

class DemoManager
{
public:
    static Demo2dShapes m_Demo2dShapes;
    static Demo3dShapes m_Demo3dShapes;

public:
    static void setNext(Demo *newDemo);
    static bool isChanged();
    static void triggerNext();

private:
    static bool m_IsDemoChanged;
    static Demo *m_CurrentDemo;
};
