#include "camera.hpp"
#include <iostream>
#include <bgfx/bgfx.h>
#include <bx/math.h>

#include "../constants.hpp"
#include "../events/eventTags.hpp"
#include "../events/eventDispatcher.hpp"
#include "../IOCContainer.hpp"

namespace camera {
const std::string k_iocTag = "mainCamera";

void orbit(Camera& camera, float dx, float dy) {
    camera._orbit[0] += dx;
    camera._orbit[1] += dy;
}

void dolly(Camera& camera, float dz) {
    const float cnear = 1.0f;
    const float cfar  = 100.0f;

    const bx::Vec3 toTarget     = bx::sub(camera._target._destination, camera._pos._destination);
    const float toTargetLen     = bx::length(toTarget);
    const float invToTargetLen  = 1.0f / (toTargetLen + bx::kFloatMin);
    const bx::Vec3 toTargetNorm = bx::mul(toTarget, invToTargetLen);

    float delta  = toTargetLen * dz;
    float newLen = toTargetLen + delta;
    if ( (cnear  < newLen || dz < 0.0f)
    &&   (newLen < cfar   || dz > 0.0f) ) {
        camera._pos._destination = bx::mad(toTargetNorm, delta, camera._pos._destination);
    }
}

void consumeOrbit(Camera& camera, float _amount) {
    float consume[2];
    consume[0] = camera._orbit[0] * _amount;
    consume[1] = camera._orbit[1] * _amount;
    camera._orbit[0] -= consume[0];
    camera._orbit[1] -= consume[1];

    const bx::Vec3 toPos     = bx::sub(camera._pos._current, camera._target._current);
    const float toPosLen     = bx::length(toPos);
    const float invToPosLen  = 1.0f / (toPosLen + bx::kFloatMin);
    const bx::Vec3 toPosNorm = bx::mul(toPos, invToPosLen);

    float ll[2];
    bx::toLatLong(&ll[0], &ll[1], toPosNorm);
    ll[0] += consume[0];
    ll[1] -= consume[1];
    ll[1]  = bx::clamp(ll[1], 0.02f, 0.98f);

    const bx::Vec3 tmp  = bx::fromLatLong(ll[0], ll[1]);
    const bx::Vec3 diff = bx::mul(bx::sub(tmp, toPosNorm), toPosLen);

    camera._pos._current = bx::add(camera._pos._current, diff);
    camera._pos._destination = bx::add(camera._pos._destination, diff);
}

void envViewMtx(Camera& camera, float* mtx) {
    const bx::Vec3 toTarget     = bx::sub(camera._target._current, camera._pos._current);
    const float toTargetLen     = bx::length(toTarget);
    const float invToTargetLen  = 1.0f / (toTargetLen + bx::kFloatMin);
    const bx::Vec3 toTargetNorm = bx::mul(toTarget, invToTargetLen);

    const bx::Vec3 right = bx::normalize(bx::cross({ 0.0f, 1.0f, 0.0f }, toTargetNorm) );
    const bx::Vec3 up    = bx::normalize(bx::cross(toTargetNorm, right) );

    mtx[ 0] = right.x;
    mtx[ 1] = right.y;
    mtx[ 2] = right.z;
    mtx[ 3] = 0.0f;
    mtx[ 4] = up.x;
    mtx[ 5] = up.y;
    mtx[ 6] = up.z;
    mtx[ 7] = 0.0f;
    mtx[ 8] = toTargetNorm.x;
    mtx[ 9] = toTargetNorm.y;
    mtx[10] = toTargetNorm.z;
    mtx[11] = 0.0f;
    mtx[12] = 0.0f;
    mtx[13] = 0.0f;
    mtx[14] = 0.0f;
    mtx[15] = 1.0f;
}

void reset(Camera& camera) {
    camera._target._current = { 0.0f, 0.0f, 0.0f };
    camera._target._destination = { 0.0f, 0.0f, 0.0f };

    camera._pos._current = {0.0f, 20.0f, -20.0f};
    camera._pos._destination = {0.0f, 20.0f, -20.0f};

    camera._orbit[0] = 0.0f;
    camera._orbit[1] = 0.0f;
}

void init(gameObject::GameObject& object, Camera& camera) {
    reset(camera);
}

void fixedUpdate(gameObject::GameObject& object, Camera& camera, float deltaTimeInSeconds) {
    camera._target._current
        = moveTowards(
            camera._target._current
            , camera._target._destination
            , deltaTimeInSeconds * 80.0f);

    camera._pos._current
        = moveTowards(
            camera._pos._current
            , camera._pos._destination
            , deltaTimeInSeconds * 80.0f);
}

void update(gameObject::GameObject& object, Camera& camera, float deltaTimeInSeconds) {

    bx::mtxLookAt(camera._view, camera._pos._current, camera._target._current, camera._up._current);

    bx::Vec3 forward = camera::cameraForward();
    bx::Vec3 right = camera::cameraRight();
    camera._up._current = bx::cross(forward, right);

    bx::mtxProj(camera._proj
        , 60.0f
        , float(constants::k_width)/float(constants::k_height)
        , 0.1f
        , 100.0f
        , bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, camera._view, camera._proj);
    bgfx::setViewRect(0, 0, 0, constants::k_width, constants::k_height);
}

void destroy(gameObject::GameObject& object, Camera& camera) {
    IOCContainer::unregisterInstance(k_iocTag);
}

// Create a camera as a gameObject instance
gameObject::GameObject createInstance() {
    Camera camera;
    const gameObject::GameObject& gameObject = gameObject::createInstance<Camera>(init, fixedUpdate, update, destroy, camera);
    return gameObject;
}

Camera& getCamera(gameObject::GameObject& gameObject) {
    return std::any_cast<Camera&>(gameObject._content);
}

Camera& getMainCamera() {
    return getCamera(*IOCContainer::getInstance<std::shared_ptr<gameObject::GameObject>>(k_iocTag).get());
}

bx::Vec3 mainCameraPosition() {
    return getMainCamera()._pos._current;
}

bx::Vec3 cameraForward(Camera& camera) {
    return {camera._view[2], camera._view[6], camera._view[10]};
}

bx::Vec3 cameraUp(Camera& camera) {
    return {camera._view[1], camera._view[5], camera._view[9]};
}

bx::Vec3 cameraRight(Camera& camera) {
    return {camera._view[0], camera._view[4], camera._view[8]};
}

bool moving(Camera& camera) {
    return (bx::length(bx::sub(camera._pos._destination, camera._pos._current)) > constants::k_epsilon) 
        || (bx::length(bx::sub(camera._target._destination, camera._target._current)) > constants::k_epsilon);
}

scene::Scene createMainCameraSceneInstance(sceneManager::SceneManager& sceneManager) {
    scene::Scene scene;
    // Create a main camera
    std::shared_ptr<gameObject::GameObject> mainCamera = scene::addObject(scene, createInstance());
    Camera& camera = getCamera(*mainCamera.get());

    // Register the main camera
    IOCContainer::registerInstance<std::shared_ptr<gameObject::GameObject>>(mainCamera, k_iocTag);
    return scene;
}
}