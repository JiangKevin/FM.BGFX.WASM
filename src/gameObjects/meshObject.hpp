#pragma once

#include <bx/math.h>
#include "bgfxExtensions/mesh.hpp"
#include "core/gameObject.hpp"
#include "core/material.hpp"
#include "core/transform.hpp"

namespace meshObject {

struct MeshObject {
    Mesh* _meshInstance = nullptr;
    transform::Transform _transform;
    // Contains the material
    Material _material;
    // Path to a .bin mesh
    std::string _meshPath = "";
};

void init(gameObject::GameObject& object, MeshObject& mesh);
void fixedUpdate(gameObject::GameObject& object, MeshObject& mesh, float deltaTimeInSeconds);
void update(gameObject::GameObject& object, MeshObject& mesh, float deltaTimeInSeconds);
void destroy(gameObject::GameObject& object, MeshObject& mesh);

MeshObject createMeshInstance(const std::string& path
    , const Material& material
    , bx::Vec3 position = transform::k_defaultPosition
    , bx::Vec3 scale = transform::k_defaultScale
    , bx::Vec3 rotation = transform::k_defaultRotation);

gameObject::GameObject createInstance(const std::string& path
    , const Material& material
    , bx::Vec3 position = transform::k_defaultPosition
    , bx::Vec3 scale = transform::k_defaultScale
    , bx::Vec3 rotation = transform::k_defaultRotation);
}