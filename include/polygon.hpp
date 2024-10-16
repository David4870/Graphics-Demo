#pragma once

#include <vector>

struct Polygon
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

Polygon polygonCreate(float n, float a, float b, float r);
