#include <cmath>
#include <numbers>

#include "shapes/shapes.hpp"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include "shapes/Sphere.h"
#include "shapes/Torus.h"

Polygon polygon2dCreate(float n, float a, float b, float r)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float theta = std::numbers::pi / 2;
    float dTheta = 2 * std::numbers::pi / n;

    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i < n; i++)
    {
        theta += dTheta;
        vertices.push_back(a + r * cos(theta));
        vertices.push_back(b + r * sin(theta));
        vertices.push_back(0.0f);
    }

    for (int i = 0; i < n; i++)
    {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    indices[indices.size() - 1] = 1;

    return { vertices, indices };
}

Polygon prismCreate(float baseRadius, float topRadius, float height, int sectors, int stacks, bool smooth, int upAxis)
{
    Cylinder cylinder(baseRadius, topRadius, height, sectors, stacks, smooth, upAxis);
    return { cylinder.vertices, cylinder.indices };
}