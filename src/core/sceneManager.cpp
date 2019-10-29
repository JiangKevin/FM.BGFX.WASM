#include "sceneManager.hpp"
#include "camera.hpp"
#include "scenes/mainMenu.hpp"
#include "IOCContainer.hpp"

namespace sceneManager {
std::string k_sceneManagerTag = "sceneManagerTag";  

void loadStartScene(SceneManager& manager) {
    scene::Scene cameraScene = camera::createMainCameraSceneInstance(manager);
    pushScene(manager, cameraScene);
    scene::Scene mainMenu = mainMenu::createInstance();
    pushScene(manager, mainMenu);
}

void pushScene(SceneManager& manager, scene::Scene& scene) {
    manager._scenes.emplace_back(std::make_unique<scene::Scene>(scene));
}

void pushScene(scene::Scene& scene) {
    sceneManager::SceneManager& sceneManager = IOCContainer::getInstance<sceneManager::SceneManager>();
    pushScene(sceneManager, scene);
}

void popScene(SceneManager& manager) {
    manager._scenes.erase(manager._scenes.begin());
}

void init(SceneManager& manager) {
    loadStartScene(manager);
    for(size_t i = 0; i < manager._scenes.size(); i++) {
        auto& scene = manager._scenes.at(i);
        init(*scene.get());
    }
}

void fixedUpdate(SceneManager& manager, float deltaTimeInSeconds) {
    for(auto& scene : manager._scenes) {
        fixedUpdate(*scene.get(), deltaTimeInSeconds);
    }
}

void update(SceneManager& manager, float deltaTimeInSeconds) {
    for(auto& scene : manager._scenes) {
        update(*scene.get(), deltaTimeInSeconds);
    }
}

void destroy(SceneManager& manager) {
    for(auto& scene : manager._scenes) {
        destroy(*scene.get());
    }
}

}