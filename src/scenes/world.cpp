#include "world.hpp"

#include "IOCContainer.hpp"
#include "constants.hpp"

#include "core/camera.hpp"
#include "representation/meshGrid.hpp"
#include "materials/wireframe.hpp"
#include "gameObjects/meshObject.hpp"

namespace world {

const std::string k_navmeshQueryRequest = "navmeshQueryRequest";
const std::string k_navmeshFindRequest  = "navmeshFindRequest";

struct World {
    std::string _levelName = "";
    std::string _navmeshName = "";

    Mesh* _navmesh;
    meshGrid::MeshGrid _grid;
};

meshGrid::MeshPoint queryNearestPoint(const bx::Vec3& positionToQuery) {
    return IOCContainer::getInstance<std::function<meshGrid::MeshPoint(const bx::Vec3&)>>(k_navmeshQueryRequest)
        (positionToQuery);   
}

meshGrid::MeshPoint findNearestPoint(triangle::Triangle& currentTriangle, const bx::Vec3& positionToQuery) {
    return IOCContainer::getInstance<std::function<meshGrid::MeshPoint(triangle::Triangle&, const bx::Vec3&)>>(k_navmeshFindRequest)
        (currentTriangle, positionToQuery);
}

// We reject navmesh queries that move any agent further than these values per axis
bool validPoint(const bx::Vec3& previous, const bx::Vec3& newPoint, const bx::Vec3& searchRange) {
    const bx::Vec3& direction =  bx::sub(newPoint, previous);

    if(bx::abs(direction.x) >= bx::abs(searchRange.x)
    || bx::abs(direction.y) >= bx::abs(searchRange.y)
    || bx::abs(direction.z) >= bx::abs(searchRange.z)) {
        return false;
    }
    return true;
}

void init(scene::Scene& scene, World& world) {
    const std::string& levelPath   = world._levelName + ".bin";
    const std::string& navmeshPath = world._navmeshName + ".bin";

    const gameObject::GameObject& levelObject = meshObject::createInstance(levelPath, wireframe::createInstance());
    scene::addObject(scene, levelObject);

    world._navmesh = meshLoad((constants::k_meshLocation + navmeshPath).c_str(), true);
    world._grid = meshGrid::createGrid(*world._navmesh);

    IOCContainer::registerInstance<std::function<meshGrid::MeshPoint(const bx::Vec3&)>>(
        [&world](const bx::Vec3& positionToQuery) -> meshGrid::MeshPoint {
            triangle::Triangle& triangle = meshGrid::getClosestTriangle(world._grid, positionToQuery);

            Ray ray = {bx::add(positionToQuery, triangle._surfaceNormal), bx::mul(triangle._surfaceNormal, -1.0f)};
            return  meshGrid::raycastRay(triangle, world._grid, ray);
        }
    , k_navmeshQueryRequest);
    
    IOCContainer::registerInstance<std::function<meshGrid::MeshPoint(triangle::Triangle&, const bx::Vec3&)>>(
        [&world](triangle::Triangle& currentTriangle, const bx::Vec3& positionToQuery) -> meshGrid::MeshPoint {
            Ray ray = {bx::add(positionToQuery, currentTriangle._surfaceNormal), bx::mul(currentTriangle._surfaceNormal, -1.0f)};
            return  meshGrid::raycastRay(currentTriangle, world._grid, ray);
        }
    , k_navmeshFindRequest);
}

void fixedUpdate(scene::Scene& scene, World& world, float deltaTimeInSeconds) {

}

void update(scene::Scene& scene, World& world, float deltaTimeInSeconds) {

}

void destroy(scene::Scene& scene, World& world) {
    meshUnload(world._navmesh);
    IOCContainer::unregisterInstance(k_navmeshQueryRequest);
    IOCContainer::unregisterInstance(k_navmeshFindRequest);
}

scene::Scene createInstance(const std::string& level, const std::string& navmesh) {
    World world;
    world._levelName = level;
    world._navmeshName = navmesh;

    return scene::createInstance<World>(init, fixedUpdate, update, destroy, world);
}
}