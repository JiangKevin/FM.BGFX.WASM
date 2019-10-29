/*
  game.cpp handles main game loop and service/lib initialization
*/
#include "game.hpp"

#include <iostream>

// BGFX and BX 
#include <bgfx/platform.h>
#include <bgfx/bgfx.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <bx/timer.h>

// IMGUI
#include "imgui/imgui.h"

#include "core/camera.hpp"
#include "core/sceneManager.hpp"
#include "events/eventTags.hpp"
#include "events/eventDispatcher.hpp"
#include "constants.hpp"
#include "IOCContainer.hpp"
#include "input.hpp"

namespace game {

void initialize() {

    #if !BX_PLATFORM_EMSCRIPTEN
        bgfx::renderFrame();
    #endif

	bgfx::Init init;
	init.type = bgfx::RendererType::OpenGL;
    if(!bgfx::init(init)) {
        std::cout << "FAILED_BGFX_INIT" << std::endl;
    }

    bgfx::reset(constants::k_width, constants::k_height, BGFX_RESET_NONE);
    bgfx::setDebug(BGFX_DEBUG_TEXT);
    bgfx::setViewClear(0,
        BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH,
        0x303030ff,
        1.f,
        0
    );
    std::cout << "BGFX INIT DONE" << std::endl;

    imguiCreate();
}

void registerInstances() {
    IOCContainer::registerInstance<events::EventDispatcher>(events::EventDispatcher());
    IOCContainer::registerInstance<sceneManager::SceneManager>(sceneManager::SceneManager());
}

void unregisterInstances() {
    IOCContainer::unregisterInstances();
}

void update() {
    // Prepare Rendering
    initialize();

    registerInstances();

    // Register required instances
    events::EventDispatcher& dispatcher = IOCContainer::getInstance<events::EventDispatcher>();
    sceneManager::SceneManager& sceneManager = IOCContainer::getInstance<sceneManager::SceneManager>();

    // TODO review this event
    bool exit = false;
    events::registerEvent(dispatcher, events::k_quitEvent, [&exit]() {
        exit = true;
    });
    
    std::cout << "INIT" << std::endl;

    // Scene, gameObjects, Shader creation, mesh loading e.t.c..
    sceneManager::init(sceneManager);

    input::InputState inputState;

    int64_t last = bx::getHPCounter();
    const double frameRateInSec = 1.0 / 60.0;
	while (!exit) {

        // Frame rate calculation
        const int64_t now = bx::getHPCounter();
		const int64_t frameTime = now - last;
		
		const double freq = double(bx::getHPFrequency());
        const float deltaTimeSec = float(double(frameTime)/freq);

        if (deltaTimeSec < frameRateInSec) {
            continue;
        }

        // Update frame time
        last = now;

        // Debug
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x4f, constants::k_gameName.c_str());

        // Setup view.
        bgfx::setViewRect(0, 0, 0, constants::k_width, constants::k_height );
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

        // bgfx related preparations
		bgfx::touch(0);

        // Handle all input
        input::handleInput(dispatcher, inputState);

        imguiBeginFrame(inputState.mouseX, inputState.mouseY
          , (inputState.leftButtonPressed ? IMGUI_MBUT_LEFT : 0)
          | (inputState.rightButtonPressed ? IMGUI_MBUT_RIGHT : 0)
          | (inputState.middleButtonPressed ? IMGUI_MBUT_MIDDLE : 0)
          , 0
          , uint16_t(constants::k_width)
          , uint16_t(constants::k_height)
        );

        // Game object logic and physics operations
        sceneManager::fixedUpdate(sceneManager, deltaTimeSec);

        // Display stats.
        ImGui::SetNextWindowPos(
          ImVec2(constants::k_width - constants::k_width / 5.0f - 10.0f, 10.0f)
          , ImGuiCond_FirstUseEver
        );
        ImGui::SetNextWindowSize(
          ImVec2(constants::k_width / 5.0f, constants::k_height / 4.0f)
          , ImGuiCond_FirstUseEver
        );

        ImGui::Begin("Stats"
          , NULL
          , 0
        );

        ImGui::Text("Frame:");        
        ImGui::SameLine(100); 
        ImGui::Text("% F/S", 1.0 / double(deltaTimeSec));

        ImGui::End();

        // UI render
        imguiEndFrame();

        // Game object Update before swap buffers
        sceneManager::update(sceneManager, deltaTimeSec);

        // Render!! swap buffers
		bgfx::frame();
	}

    // Cleanup
    sceneManager::destroy(sceneManager);
    
    unregisterInstances();

    imguiDestroy();

	#if BX_PLATFORM_EMSCRIPTEN
    	emscripten_cancel_main_loop();
	#endif

    unregisterEvent(dispatcher, events::k_quitEvent);

    bgfx::shutdown();
}

void initializeGame() {
    #if BX_PLATFORM_EMSCRIPTEN
        emscripten_set_main_loop(update, -1, 1);
    #else
        update();
    #endif
}
}