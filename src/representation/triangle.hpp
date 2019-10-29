#pragma once

#include <functional>
#include <vector>

#include <bx/math.h>

#include "bgfxExtensions/bounds.h"

// TOOD Move to vertex header
struct Vertex {
    bx::Vec3 _vertex = {0.0f, 0.0f, 0.0f};
    uint16_t _index  = 0;
};

namespace triangle {

struct Triangle {
    std::vector<Vertex> _verts = {
        Vertex(),
        Vertex(),
        Vertex()
    };
    std::vector<uint32_t> _neighbors;
    bx::Vec3 _surfaceNormal = {0.0f, 0.0f, 0.0f};
};

extern triangle::Triangle s_defaultTriangle;

bx::Vec3 surfaceNormal(Triangle& triangle);
bx::Vec3 barycentricUVW(Triangle& triangle, const bx::Vec3 &point);
bool barycentricInsideTriangle(const bx::Vec3 &barycentric);
inline float triArea2D(float x1, float y1, float x2, float y2, float x3, float y3);
bool positionInsideTriangle(Triangle& triangleReference, bx::Vec3 query);
bx::Vec3 closestPoint(Triangle& triangle, const bx::Vec3 &p);
bool intersects(Triangle& triangle, const Ray &r, float *d, bx::Vec3 *intersectionPoint);
}