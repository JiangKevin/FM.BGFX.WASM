#include "meshGrid.hpp"

#include <iostream>

#include "cppExtensions.hpp"

namespace meshGrid {

bx::Vec3 getVertex(Mesh& mesh, Group& group, uint16_t index) {
    float pos[4];
    vertexUnpack(pos, bgfx::Attrib::Position, mesh.m_decl, group.m_vertices, index);
    return {pos[0], pos[1], pos[2]};
}

std::string toString(bx::Vec3 vec) {
    return cstd::to_string(vec.x) + cstd::to_string(vec.y) + cstd::to_string(vec.z);
}

MeshGrid createGrid(Mesh& mesh) {
    MeshGrid grid;
    
    // Map that links each unique vertex index with a list of triangles that use it
    std::unordered_map<std::string, std::vector<uint32_t>> trianglesForEachIndex;
    // Obtain all the triangles from the mesh
    for(Group& group : mesh.m_groups) {
        grid._triangles.reserve(grid._triangles.size() + (size_t)(group.m_numIndices / 3.0f));
        for(uint32_t index = 0, triangleIndex = 0; index < group.m_numIndices; index +=3, ++triangleIndex) {
            uint16_t index0 = group.m_indices[index];
            uint16_t index1 = group.m_indices[index+1];
            uint16_t index2 = group.m_indices[index+2];

            bx::Vec3 a = getVertex(mesh, group, index0);
            bx::Vec3 b = getVertex(mesh, group, index1);
            bx::Vec3 c = getVertex(mesh, group, index2);

            triangle::Triangle triangle;
            triangle._verts = {
                { a, index0},
                { b, index1},
                { c, index2}
            };

            triangle._surfaceNormal = triangle::surfaceNormal(triangle);
            
            // Store the value
            grid._triangles.emplace_back(triangle);

            // Store a reference to this triangle
            trianglesForEachIndex[toString(a)].emplace_back(triangleIndex);
            trianglesForEachIndex[toString(b)].emplace_back(triangleIndex);
            trianglesForEachIndex[toString(c)].emplace_back(triangleIndex);
        }
    }

    for(triangle::Triangle& triangle : grid._triangles) {
        for(Vertex& vertex : triangle._verts) {
            std::vector<uint32_t> indexTriangles = trianglesForEachIndex[toString(vertex._vertex)];
        
            for(uint32_t triangleIndex : indexTriangles) {
                if(std::find(triangle._neighbors.begin(), triangle._neighbors.end(), triangleIndex) != triangle._neighbors.end() ) {
                    continue;
                }
                triangle._neighbors.emplace_back(triangleIndex);
            }
        
        }
    }

    return grid;
}

std::reference_wrapper<triangle::Triangle> getClosestTriangle(MeshGrid& grid, bx::Vec3 query) {
    for(triangle::Triangle& triangle : grid._triangles) {
        bx::Vec3 uvw = triangle::barycentricUVW(triangle, query);
        if(triangle::barycentricInsideTriangle(uvw)) {
            return triangle;
        }
    }

    return triangle::s_defaultTriangle;
}

MeshPoint raycastRay(triangle::Triangle& triangleReference, MeshGrid& grid, const Ray& ray) {
    float distance;
    bx::Vec3 intersectionPoint;
    bool intersect = triangle::intersects(triangleReference, ray, &distance, &intersectionPoint);

    if(intersect) {
        return {triangleReference, intersectionPoint};
    }

    // Lets check our neighbors
    for(uint32_t triangleIndex : triangleReference._neighbors) {
        triangle::Triangle& triangle = getTriangle(grid, triangleIndex);

        intersect = triangle::intersects(triangle, ray, &distance, &intersectionPoint);

        if(intersect) {
            return {triangle, intersectionPoint};
        }
    }
    return {triangleReference, triangle::closestPoint(triangleReference, ray.pos)};
}

MeshPoint findNearestPosition(triangle::Triangle& triangleReference, MeshGrid& grid, const bx::Vec3 query) {
    // Check the current triangle to see if we are there
    if(triangle::positionInsideTriangle(triangleReference, query)) {
        return {triangleReference, triangle::closestPoint(triangleReference, query)};
    }

    triangle::Triangle& shortestTriangle = triangleReference;
    // TODO potential peformance improvement only do closestPoint if we don't update the variables within the for loop
    bx::Vec3 closestPoint = triangle::closestPoint(triangleReference, query);
    float shortestGoalDifference = bx::kFloatMax;

    // Lets check our neighbors
    for(uint32_t triangleIndex : triangleReference._neighbors) {
        triangle::Triangle& triangle = getTriangle(grid, triangleIndex);

        bx::Vec3 newClosestPoint = triangle::closestPoint(triangle, query);
        float length = bx::length(bx::sub(query, newClosestPoint));

        if(length < shortestGoalDifference) {
            shortestGoalDifference = length;
            shortestTriangle = triangle;
            closestPoint = newClosestPoint;
        }
    }
    return {shortestTriangle, closestPoint};
}

std::reference_wrapper<triangle::Triangle> getTriangle(MeshGrid& grid, uint32_t index) {
    return std::ref(grid._triangles[index]);
}
}