#include "mainMenu.hpp"
#include "materials/wireframe.hpp"
#include "gameObjects/meshObject.hpp"
#include "gameObjects/player.hpp"
#include "core/sceneManager.hpp"
#include "gameScene.hpp"

namespace mainMenu {

struct MainMenu {

};

void init(scene::Scene& scene, MainMenu& mainMenu) {
    //TODO there should be a button to push the game scene and pop the main menu
    scene::Scene gameScene = gameScene::createInstance();
    sceneManager::pushScene(gameScene);
}

void fixedUpdate(scene::Scene& scene, MainMenu& game, float deltaTimeInSeconds) {

}

void update(scene::Scene& scene, MainMenu& mainMenu, float deltaTimeInSeconds) {

}

void destroy(scene::Scene& scene, MainMenu& mainMenu) {

}

scene::Scene createInstance() {
    MainMenu mainMenu;
    return scene::createInstance<MainMenu>(init, fixedUpdate, update, destroy, mainMenu);
}
}