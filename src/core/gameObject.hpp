#pragma once

#include <any>
#include <functional>

namespace gameObject {
/* A gameObject is something that exists inside a scene that is:
- Initialized
- Updated
- Deleted
*/
struct GameObject {
    std::function<void(GameObject& gameObject)> 
      _customInit = [](GameObject& gameObject){};
    std::function<void(GameObject& gameObject, float deltaTimeInSeconds)> 
      _customFixedUpdate = [](GameObject& gameObject, float deltaTimeInSeconds){};
    std::function<void(GameObject& gameObject, float deltaTimeInSeconds)> 
      _customUpdate = [](GameObject& gameObject, float deltaTimeInSeconds){};
    std::function<void(GameObject& gameObject)> _customDestroy = [](GameObject& gameObject){};
    std::any _content;
};

// Init is called only once
void init(GameObject&);

// Fixed Update is called every frame
void fixedUpdate(GameObject&, float deltaTimeInSeconds);

// Update is called every frame
void update(GameObject&, float deltaTimeInSeconds);

// Destroy is called at the end of the execution
void destroy(GameObject&);

// Template for gameObject construction for other entities to use
template<typename T> GameObject createInstance(
      std::function<void(gameObject::GameObject&, T&)> customInit
    , std::function<void(gameObject::GameObject&, T&, float)> customFixedUpdate
    , std::function<void(gameObject::GameObject&, T&, float)> customUpdate
    , std::function<void(gameObject::GameObject&, T&)> customDestroy
    , T& customObject) {

    GameObject gameObject;
    gameObject._content = std::make_any<T>(customObject);
    gameObject._customInit = [customInit](gameObject::GameObject& gameObject) {
        customInit(gameObject, std::any_cast<T&>(gameObject._content));
    };
    gameObject._customFixedUpdate = [customFixedUpdate](gameObject::GameObject& gameObject, float deltaTimeInSeconds) {
        customFixedUpdate(gameObject, std::any_cast<T&>(gameObject._content), deltaTimeInSeconds);
    };
    gameObject._customUpdate = [customUpdate](gameObject::GameObject& gameObject, float deltaTimeInSeconds) {
        customUpdate(gameObject, std::any_cast<T&>(gameObject._content), deltaTimeInSeconds);
    };
    gameObject._customDestroy = [customDestroy](gameObject::GameObject& gameObject) {
        customDestroy(gameObject, std::any_cast<T&>(gameObject._content));
    };
    return gameObject;
}
}