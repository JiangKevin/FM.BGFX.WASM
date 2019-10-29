#include <iostream>

#include "entity.hpp"

#include "IOCContainer.hpp"
#include "events/eventDispatcher.hpp"
#include "events/eventTags.hpp"
#include "core/camera.hpp"

#include "constants.hpp"

void init(gameObject::GameObject& object, Entity& entity) {
    meshObject::init(object, entity._meshObject);
}

void start(Entity& entity) {
    if (entity._started) {
        return;
    }

    // Sample where we are
    entity._lastSampledMeshPoint = world::queryNearestPoint(entity._meshObject._transform._position._current);

    // Sample a forward location
    bx::Vec3 newDestination = bx::add(entity._lastSampledMeshPoint._position, transform::forward(entity._meshObject._transform));
    meshGrid::MeshPoint navmeshPoint = world::findNearestPoint(entity._lastSampledMeshPoint._triangle, newDestination);

    // Set entity position
    entity._meshObject._transform._position._current = entity._lastSampledMeshPoint._position;
    entity._meshObject._transform._position._destination = entity._meshObject._transform._position._current;

    // Set orientation
    entity._meshObject._transform._orientation._current
        = LookRotation(
            entity._lastSampledMeshPoint._position
            , navmeshPoint._position
            , entity._lastSampledMeshPoint._triangle.get()._surfaceNormal);

    entity._meshObject._transform._orientation._destination = entity._meshObject._transform._orientation._current;

    // TODO remove this
    entity._started = true;
}

void fixedUpdate(gameObject::GameObject& object, Entity& entity, float deltaTimeInSeconds) {

    // TODO add a start method to all game objects
    start(entity);

    bx::Vec3 forwardDirection = camera::cameraUp();
    bx::Vec3 rightDirection = camera::cameraRight();
    bx::Vec3 cameraDirection = bx::add(
        bx::mul(forwardDirection, entity._movingDirection.z),
        bx::mul(rightDirection, entity._movingDirection.x));

    if (bx::length(cameraDirection) > constants::k_epsilon && !transform::moving(entity._meshObject._transform, deltaTimeInSeconds * entity._meshObject._transform._speed)) {
        cameraDirection = bx::mul(bx::normalize(cameraDirection), 1.0f);

        bx::Vec3 newDestination = bx::add(entity._meshObject._transform._position._current, cameraDirection);

        // Sample next position
        meshGrid::MeshPoint navmeshPoint = world::findNearestPoint(entity._lastSampledMeshPoint._triangle, newDestination);

        entity._lastSampledMeshPoint = navmeshPoint;

        transform::setDirection(entity._meshObject._transform, navmeshPoint._position);
        if (!transform::rotating(entity._meshObject._transform)) {
            transform::setRotation(entity._meshObject._transform,
                LookRotation(
                    entity._meshObject._transform._position._current,
                    navmeshPoint._position,
                    navmeshPoint._triangle.get()._surfaceNormal));
        }

    }

    // This is where the transform will get updated
    meshObject::fixedUpdate(object, entity._meshObject, deltaTimeInSeconds);
}

void update(gameObject::GameObject& object, Entity& entity, float deltaTimeInSeconds) {
    meshObject::update(object, entity._meshObject, deltaTimeInSeconds);
}

void destroy(gameObject::GameObject& object, Entity& entity) {
    meshObject::destroy(object, entity._meshObject);
}