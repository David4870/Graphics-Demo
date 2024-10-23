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
    std::vector<float> normalVertices;
    int size = cylinder.vertices.size();

    for (int i = 0; i < size; i += 3)
    {
        normalVertices.push_back(cylinder.vertices[i]);
        normalVertices.push_back(cylinder.vertices[i+1]);
        normalVertices.push_back(cylinder.vertices[i+2]);

        normalVertices.push_back(cylinder.normals[i]);
        normalVertices.push_back(cylinder.normals[i+1]);
        normalVertices.push_back(cylinder.normals[i+2]);
    }

    return { normalVertices, cylinder.indices };
}

Polygon coneCreate(float baseRadius, float height, int sectors, int stacks, bool smooth, int upAxis)
{
    Cone cone(baseRadius, height, sectors, stacks, smooth, upAxis);
    return { cone.vertices, cone.indices };
}

Polygon sphereCreate(float radius, int sectors, int stacks, bool smooth, int upAxis)
{
    Sphere sphere(radius, sectors, stacks, smooth, upAxis);
    std::vector<float> normalVertices;
    int size = sphere.vertices.size();

    for (int i = 0; i < size; i += 3)
    {
        normalVertices.push_back(sphere.vertices[i]);
        normalVertices.push_back(sphere.vertices[i+1]);
        normalVertices.push_back(sphere.vertices[i+2]);

        normalVertices.push_back(sphere.normals[i]);
        normalVertices.push_back(sphere.normals[i+1]);
        normalVertices.push_back(sphere.normals[i+2]);
    }

    return { normalVertices, sphere.indices };
}

Polygon torusCreate(float majorRadius, float minorRadius, int sectors, int sides, bool smooth, int upAxis)
{
    Torus torus(majorRadius, minorRadius, sectors, sides, smooth, upAxis);
    std::vector<float> normalVertices;
    int size = torus.vertices.size();

    for (int i = 0; i < size; i += 3)
    {
        normalVertices.push_back(torus.vertices[i]);
        normalVertices.push_back(torus.vertices[i+1]);
        normalVertices.push_back(torus.vertices[i+2]);

        normalVertices.push_back(torus.normals[i]);
        normalVertices.push_back(torus.normals[i+1]);
        normalVertices.push_back(torus.normals[i+2]);
    }

    return { normalVertices, torus.indices };
}