#include "scene.hpp"

namespace scene {
void init(Scene& scene) {
    scene._customInit(scene);
    for(auto& object : scene._objects) {
        gameObject::init(*object.get());
    }
}

void fixedUpdate(Scene& scene, float deltaTimeInSeconds) {
    scene._customFixedUpdate(scene, deltaTimeInSeconds);
    for(auto& object : scene._objects) {
        gameObject::fixedUpdate(*object.get(), deltaTimeInSeconds);
    }
}

void update(Scene& scene, float deltaTimeInSeconds) {
    scene._customUpdate(scene, deltaTimeInSeconds);
    for(auto& object : scene._objects) {
        gameObject::update(*object.get(), deltaTimeInSeconds);
    }
}

void destroy(Scene& scene) {
    scene._customDestroy(scene);
    for(auto& object : scene._objects) {
        gameObject::destroy(*object.get());
    }
}

std::shared_ptr<gameObject::GameObject> addObject(Scene& scene, const gameObject::GameObject& object) {
    return scene._objects.emplace_back(std::make_shared<gameObject::GameObject>(object));
}

}