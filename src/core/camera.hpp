#pragma once

#include "mathExtensions.hpp"
#include "sceneManager.hpp"
#include "gameObject.hpp"
#include "scene.hpp"
#include "constants.hpp"

namespace camera {

struct Camera {
    InterpTransform3f _target    = { constants::k_empty, constants::k_empty};
    InterpTransform3f _pos       = { constants::k_empty, constants::k_empty};
    InterpTransform3f _up        = { {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
    float _orbit[2]              = {0};

    float _view[16]              = {0};
    float _proj[16]              = {0};
};

Camera&   getMainCamera();
bx::Vec3  mainCameraPosition();

// Directly acess the view matrix for the up vector, look at mtxLookAt implementation
bx::Vec3 cameraForward(Camera& camera = getMainCamera());

// Directly acess the view matrix for the up vector, look at mtxLookAt implementation
bx::Vec3 cameraUp(Camera& camera = getMainCamera());

// Directly acess the view matrix for the up vector, look at mtxLookAt implementation
bx::Vec3 cameraRight(Camera& camera = getMainCamera());

bool moving(Camera& camera = getMainCamera());

// Create a camera as a Scene Instance, registers it on the IOCContainer
scene::Scene    createMainCameraSceneInstance(sceneManager::SceneManager& sceneManager);
}