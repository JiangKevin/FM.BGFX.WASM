#include "gameObject.hpp"

namespace gameObject {
void init(GameObject& object) {
    object._customInit(object);
}

void fixedUpdate(GameObject& object, float deltaTimeInSeconds) {
    object._customFixedUpdate(object, deltaTimeInSeconds);
}

void update(GameObject& object, float deltaTimeInSeconds) {
    object._customUpdate(object, deltaTimeInSeconds);
}

void destroy(GameObject& object) {
    object._customDestroy(object);
}
}