#pragma once

#include "gameObject.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace scene {
struct Scene {
    std::vector<std::shared_ptr<gameObject::GameObject>> _objects;
    std::function<void(Scene& scene)> _customInit 
        = [](Scene& scene){};
    std::function<void(Scene& scene, float deltaTimeInSeconds)> _customFixedUpdate 
        = [](Scene& scene, float deltaTimeInSeconds){};
    std::function<void(Scene& scene, float deltaTimeInSeconds)> _customUpdate 
        = [](Scene& scene, float deltaTimeInSeconds){};
    std::function<void(Scene& scene)> _customDestroy 
        = [](Scene& scene){};
    std::any _content;
};

void init(Scene& scene);
void fixedUpdate(Scene& scene, float deltaTimeInSeconds);
void update(Scene& scene, float deltaTimeInSeconds);
void destroy(Scene& scene);

std::shared_ptr<gameObject::GameObject> addObject(Scene& scene, const gameObject::GameObject& object);

// Template for Scene construction for other entities to use
template<typename T> Scene createInstance(
      std::function<void(Scene&, T&)> customInit
    , std::function<void(Scene&, T&, float)> customFixedUpdate
    , std::function<void(Scene&, T&, float)> customUpdate
    , std::function<void(Scene&, T&)> customDestroy
    , T& customObject) {

    Scene scene;
    scene._content = std::make_any<T>(customObject);
    scene._customInit = [customInit](Scene& scene) {
        customInit(scene, std::any_cast<T&>(scene._content));
    };
    scene._customFixedUpdate = [customFixedUpdate](Scene& scene, float deltaTimeInSeconds) {
        customFixedUpdate(scene, std::any_cast<T&>(scene._content), deltaTimeInSeconds);
    };
    scene._customUpdate = [customUpdate](Scene& scene, float deltaTimeInSeconds) {
        customUpdate(scene, std::any_cast<T&>(scene._content), deltaTimeInSeconds);
    };
    scene._customDestroy = [customDestroy](Scene& scene) {
        customDestroy(scene, std::any_cast<T&>(scene._content));
    };
    return scene;
}
}