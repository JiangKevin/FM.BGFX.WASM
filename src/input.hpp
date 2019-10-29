#pragma once

namespace events {
    struct EventDispatcher;
}

namespace input {

    struct InputState {
        uint32_t mouseX = 0, mouseY = 0;

        bool leftButtonPressed = false;
        bool rightButtonPressed = false;
        bool middleButtonPressed = false;
    };

    void handleInput(events::EventDispatcher& dispatcher, InputState& state);
}