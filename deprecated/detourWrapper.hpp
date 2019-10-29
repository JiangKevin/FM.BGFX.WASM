#pragma once

#include <string>
#include <vector>

#include <bx/math.h>

class dtNavMesh;
class dtNavMeshQuery;

namespace detourWrapper {

struct NavmeshPoint {
    bx::Vec3 _position      = {0.0f, 0.0f, 0.0f};
    bx::Vec3 _surfaceNormal = {0.0f, 0.0f, 0.0f};

    inline bool isEmpty() const { 
        return (bx::length(_position) == 0) 
            || (bx::length(_surfaceNormal) == 0);
    }
};

// Loads a navmesh created using the recast demo app
dtNavMesh* loadNavmeshFromRecastFile(const std::string& name);

dtNavMeshQuery* createQuery(dtNavMesh* navmesh);
NavmeshPoint findNearestPoint(dtNavMeshQuery* query, bx::Vec3 position);
std::vector<NavmeshPoint> findPath(dtNavMeshQuery* query, bx::Vec3 startPosition, bx::Vec3 endPosition);
void freeQuery(dtNavMeshQuery* query);
void freeNavmesh(dtNavMesh* navmesh);
}