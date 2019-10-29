#include "meshObject.hpp"

#include "../bgfxExtensions/bgfxUtils.h"
#include "../constants.hpp"

namespace meshObject {

void init(gameObject::GameObject& object, MeshObject& mesh) {
    mesh._meshInstance = meshLoad((constants::k_meshLocation + mesh._meshPath).c_str());
    init(mesh._material);
}

void fixedUpdate(gameObject::GameObject& object, MeshObject& mesh, float deltaTimeInSeconds) {
    transform::fixedUpdate(mesh._transform, deltaTimeInSeconds);
}

void update(gameObject::GameObject& object, MeshObject& mesh, float deltaTimeInSeconds) {
    transform::update(mesh._transform, deltaTimeInSeconds);
    update(mesh._material);
    meshSubmit(mesh._meshInstance
    , 0
    , mesh._material.program
    , mesh._transform._scaleRotationTranslationMatrix
    , mesh._material.state);
}

void destroy(gameObject::GameObject& object, MeshObject& mesh) {
    meshUnload(mesh._meshInstance);
}

MeshObject createMeshInstance(const std::string& path
    , const Material& material
    , bx::Vec3 position
    , bx::Vec3 scale
    , bx::Vec3 rotation) {

    transform::Transform transform;
    transform._position = {position, position};
    transform._scale = {scale, scale};

    bx::Quaternion orientation = bx::mul(bx::mul(bx::rotateX(rotation.x), bx::rotateX(rotation.y)), bx::rotateZ(rotation.z));
    transform._orientation = {orientation, orientation, orientation};
    return {nullptr
            , transform
            , material
            , path};
}

gameObject::GameObject createInstance(const std::string& path
    , const Material& material
    , bx::Vec3 position
    , bx::Vec3 scale
    , bx::Vec3 rotation
    ) {
    MeshObject meshObject = createMeshInstance(path, material, position, scale, rotation);
    const gameObject::GameObject& gameObject = gameObject::createInstance<MeshObject>(init, fixedUpdate, update, destroy, meshObject);
    return gameObject;
}
}