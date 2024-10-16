#pragma once

#include "appContext.hpp"
#include "demo.hpp"
#include "demo2dShapes.hpp"
#include "demo3dShapes.hpp"
#include "demoRaycast.hpp"

class DemoManager
{
public:
    static bool isDemoChanged;
    static Demo *currentDemo;
    static Demo2dShapes demo2dShapes;
    static Demo3dShapes demo3dShapes;
    static DemoRaycast demoRaycast;

    static void setNext(Demo *newDemo);
    static bool isChanged();
    static void triggerNext();
};
