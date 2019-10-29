#pragma once

#include "scene.hpp"
#include <vector>

namespace sceneManager {

extern std::string k_sceneManagerTag;    
struct SceneManager {
    inline static const std::string k_iocTag = "sceneManager";
    std::vector<std::shared_ptr<scene::Scene>> _scenes;
};

void pushScene(SceneManager&, scene::Scene&);
void pushScene(scene::Scene& scene);
void popScene(SceneManager&);
void init(SceneManager&);
void fixedUpdate(SceneManager&, float deltaTimeInSeconds);
void update(SceneManager&, float deltaTimeInSeconds);
void destroy(SceneManager&);

}