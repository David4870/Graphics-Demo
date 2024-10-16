#include <cmath>
#include <numbers>

#include "polygon.hpp"

Polygon polygonCreate(float n, float a, float b, float r)
{
    Polygon polygon;

    float theta = std::numbers::pi / 2;
    float dTheta = 2 * std::numbers::pi / n;

    polygon.vertices.push_back(0.0f);
    polygon.vertices.push_back(0.0f);
    polygon.vertices.push_back(0.0f);
    for(int i = 0; i < n; i++)
    {
        theta += dTheta;
        polygon.vertices.push_back(a + r * cos(theta));
        polygon.vertices.push_back(b + r * sin(theta));
        polygon.vertices.push_back(0.0f);
    }

    for(int i = 0; i < n; i++)
    {
        polygon.indices.push_back(0);
        polygon.indices.push_back(i + 1);
        polygon.indices.push_back(i + 2);
    }
    polygon.indices[polygon.indices.size() - 1] = 1;

    return polygon;
}