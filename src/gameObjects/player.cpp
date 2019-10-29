#include "player.hpp"

#include "meshObject.hpp"
#include "IOCContainer.hpp"
#include "events/eventDispatcher.hpp"
#include "events/eventTags.hpp"
#include "core/camera.hpp"
#include "scenes/world.hpp"
#include "constants.hpp"

#include <iostream>

namespace player {

struct Player {
    Entity entity;
};

// TODO make the camera use the transform class
void updateCamera(camera::Camera& camera, Player& player) {
    camera._pos._destination = bx::add(
      player.entity._meshObject._transform._position._current,
      bx::mul(transform::up(player.entity._meshObject._transform), 20.0f));

    camera._target._destination = player.entity._meshObject._transform._position._current;
}

void init(gameObject::GameObject& object, Player& player) {
    init(object, player.entity);

    // Enable movement when user has a key pressed
    events::EventDispatcher& dispatcher = IOCContainer::getInstance<events::EventDispatcher>();
    events::registerEvent(dispatcher
        , events::k_moveW
        , [&player]() {
            player.entity._movingDirection.z = 1;
        }
    );
    events::registerEvent(dispatcher
        , events::k_moveA
        , [&player]() {
            player.entity._movingDirection.x = -1;
        }
    );
    events::registerEvent(dispatcher
        , events::k_moveS
        , [&player]() {
            player.entity._movingDirection.z = -1;
        }
    );
    events::registerEvent(dispatcher
        , events::k_moveD
        , [&player]() {
            player.entity._movingDirection.x = 1;
        }
    );

    // Disable movement when user lifts the key
    events::registerEvent(dispatcher
        , events::k_moveWup
        , [&player]() {
            player.entity._movingDirection.z = player.entity._movingDirection.z == 1 ? 0 : player.entity._movingDirection.z;
        }
    );
    events::registerEvent(dispatcher
        , events::k_moveAup
        , [&player]() {
            player.entity._movingDirection.x = player.entity._movingDirection.x == -1 ? 0 : player.entity._movingDirection.x;
        }
    );
    events::registerEvent(dispatcher
        , events::k_moveSup
        , [&player]() {
            player.entity._movingDirection.z = player.entity._movingDirection.z == -1 ? 0 : player.entity._movingDirection.z;
        }
    );
    events::registerEvent(dispatcher
        , events::k_moveDup
        , [&player]() {
            player.entity._movingDirection.x = player.entity._movingDirection.x == 1 ? 0 : player.entity._movingDirection.x;
        }
    );
    updateCamera(camera::getMainCamera(), player);
}

void fixedUpdate(gameObject::GameObject& object, Player& player, float deltaTimeInSeconds) {
    fixedUpdate(object, player.entity, deltaTimeInSeconds);
    // Update Camera based on player's position
    updateCamera(camera::getMainCamera(), player);
}

void update(gameObject::GameObject& object, Player& player, float deltaTimeInSeconds) { 
    update(object, player.entity, deltaTimeInSeconds);
}

void destroy(gameObject::GameObject& object, Player& player) {
    destroy(object, player.entity);
}

gameObject::GameObject createInstance(meshObject::MeshObject& meshObject) {
    Player player;
    player.entity._meshObject = meshObject;
    player.entity._meshObject._transform._speed = 8.0f;

    const gameObject::GameObject& gameObject = gameObject::createInstance<Player>(init, fixedUpdate, update, destroy, player);
    return gameObject;
}
}