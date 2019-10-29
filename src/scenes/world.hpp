#pragma once

#include <tuple>

#include <bx/math.h>

#include "core/scene.hpp"
#include "representation/meshGrid.hpp"

namespace world {

/*
    Query nearest mesh point, this is a expensive operation as theres no base triangle to base
    the search.
*/
meshGrid::MeshPoint queryNearestPoint(const bx::Vec3& positionToQuery);
/*
    Query nearest mesh point, this is a cheap operation as we already have a base triangle.
*/
meshGrid::MeshPoint findNearestPoint(triangle::Triangle& currentTriangle, const bx::Vec3& positionToQuery);
scene::Scene createInstance(const std::string& level, const std::string& navmesh);
}