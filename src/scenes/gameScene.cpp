#include "gameScene.hpp"

#include "constants.hpp"

#include "world.hpp"
#include "gameObjects/player.hpp"
#include "materials/wireframe.hpp"

#include "core/scene.hpp"
#include "core/sceneManager.hpp"

namespace gameScene {

struct GameScene {
    
};

void init(scene::Scene& scene, GameScene& game) {
    // Create the player
    meshObject::MeshObject meshObject = 
        meshObject::createMeshInstance("spaceLego.bin", wireframe::createInstance());
    gameObject::GameObject player = player::createInstance(meshObject);
    scene::addObject(scene, player);

    //TODO world chooser, plus this can and should be on a xml
    scene::Scene level = world::createInstance("level0", "level0_navmesh0");
    sceneManager::pushScene(level);
}

void fixedUpdate(scene::Scene& scene, GameScene& game, float deltaTimeInSeconds) {

}

void update(scene::Scene& scene, GameScene& game, float deltaTimeInSeconds) {

}

void destroy(scene::Scene& scene, GameScene& game) {

}

scene::Scene createInstance() {
    GameScene game;
    return scene::createInstance<GameScene>(init, fixedUpdate, update, destroy, game);
}
}