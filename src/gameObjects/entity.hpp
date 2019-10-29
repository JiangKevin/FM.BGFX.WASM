#pragma once

#include "meshObject.hpp"
#include "scenes/world.hpp"

/* Represents something that moves
helps provide a base for other entities like the player*/
struct Entity {
    meshGrid::MeshPoint _lastSampledMeshPoint;
    meshObject::MeshObject _meshObject;
    bx::Vec3 _movingDirection = { 0.0f, 0.0f, 0.0f };

    bool _started = false;
};

void init(gameObject::GameObject& object, Entity& entity);
void fixedUpdate(gameObject::GameObject& object, Entity& entity, float deltaTimeInSeconds);
void update(gameObject::GameObject& object, Entity& entity, float deltaTimeInSeconds);
void destroy(gameObject::GameObject& object, Entity& entity);