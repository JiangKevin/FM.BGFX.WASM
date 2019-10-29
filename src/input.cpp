#include <SDL2/SDL.h>

#include "input.hpp"
#include "events/eventDispatcher.hpp"
#include "events/eventTags.hpp"

namespace input {

    void resetState(InputState& inputState) {
        int a, b;
        SDL_GetMouseState(&a, &b);
        inputState.mouseX = a;
        inputState.mouseY = b;

        inputState.leftButtonPressed = false;
        inputState.middleButtonPressed = false;
        inputState.rightButtonPressed = false;
    }

    void handleInput(events::EventDispatcher& dispatcher, InputState& inputState) {

        resetState(inputState);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_QUIT: {
                events::postEvent(dispatcher, events::k_quitEvent);
                break;
            }
            case SDL_WINDOWEVENT: {
                const SDL_WindowEvent& wev = event.window;
                switch (wev.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    break;

                case SDL_WINDOWEVENT_CLOSE:
                    events::postEvent(dispatcher, events::k_quitEvent);
                    break;
                }
            }
            case SDL_MOUSEBUTTONDOWN: {
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    inputState.leftButtonPressed = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    inputState.rightButtonPressed = true;
                    break;
                case SDL_BUTTON_MIDDLE:
                    inputState.middleButtonPressed = true;
                    break;
                }
                break;
            }

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                case SDLK_w:
                    events::postEvent(dispatcher, events::k_moveW);
                    break;
                case SDLK_a:
                    events::postEvent(dispatcher, events::k_moveA);
                    break;
                case SDLK_s:
                    events::postEvent(dispatcher, events::k_moveS);
                    break;
                case SDLK_d:
                    events::postEvent(dispatcher, events::k_moveD);
                    break;
                }
                break;
            }
            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {
                case SDLK_w:
                    events::postEvent(dispatcher, events::k_moveWup);
                    break;
                case SDLK_a:
                    events::postEvent(dispatcher, events::k_moveAup);
                    break;
                case SDLK_s:
                    events::postEvent(dispatcher, events::k_moveSup);
                    break;
                case SDLK_d:
                    events::postEvent(dispatcher, events::k_moveDup);
                    break;
                }
                break;
            }
            }
        }
    }
}