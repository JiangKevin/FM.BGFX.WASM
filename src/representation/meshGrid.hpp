#pragma once

#include <functional>
#include <vector>
#include <unordered_map>

#include <bx/math.h>

#include "triangle.hpp"
#include "bgfxExtensions/bounds.h"
#include "bgfxExtensions/mesh.hpp"

namespace meshGrid {

struct MeshGrid {
    std::vector<triangle::Triangle> _triangles;
};

struct MeshPoint {
    std::reference_wrapper<triangle::Triangle> _triangle = triangle::s_defaultTriangle;
    bx::Vec3 _position = {0.0f, 0.0f, 0.0f};
};

/*
    Creates a grid from a bgfx loaded mesh
*/
MeshGrid createGrid(Mesh& mesh);

/* 
    Obtains the closest triangle from the query position, by iterating all triangles in the mesh
    Expensive operation
*/
std::reference_wrapper<triangle::Triangle> getClosestTriangle(MeshGrid& grid, bx::Vec3 query);

/* 
    Obtains the nearest position from doing a bread first search based on a triangle reference
*/
MeshPoint findNearestPosition(triangle::Triangle& triangleReference, MeshGrid& grid, const bx::Vec3 query);

/*
    Creates a reference to the wanted triangle    
*/
std::reference_wrapper<triangle::Triangle> getTriangle(MeshGrid& grid, uint32_t index);

MeshPoint raycastRay(triangle::Triangle& triangleReference, MeshGrid& grid, const Ray& ray);
}