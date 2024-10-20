#pragma once

#include <vector>

#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include "shapes/Sphere.h"
#include "shapes/Torus.h"

struct Polygon
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

Polygon polygon2dCreate(float n, float a, float b, float r);

Polygon prismCreate(float baseRadius, float topRadius, float height, int sectors, int stacks, bool smooth, int upAxis);