/*
  main.cpp handles SDL window creation and game initialization
*/

// Macro to allow us to have a main function, without this sdl will run its own main
#define SDL_MAIN_HANDLED

#include <bx/platform.h>
#include <bx/bx.h>
#include <bgfx/platform.h>
#include <bgfx/bgfx.h>

#if BX_PLATFORM_EMSCRIPTEN
	#include <emscripten.h>
#endif 

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "game.hpp"
#include "constants.hpp"

inline bool sdlSetWindow(SDL_Window* _window)
{
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(_window, &wmi) )
	{
		return false; 
	}

	bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX
	pd.ndt          = wmi.info.x11.display;
	pd.nwh          = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_WINDOWS
	pd.ndt          = NULL;
	pd.nwh          = wmi.info.win.window;
#endif
	pd.context      = NULL;
	pd.backBuffer   = NULL;
	pd.backBufferDS = NULL;
	bgfx::setPlatformData(pd);

	return true;
}

int main() {
	SDL_Init(SDL_INIT_EVENTS);

	SDL_Window* window = NULL;

    window = SDL_CreateWindow(constants::k_gameName.c_str(), 
    				SDL_WINDOWPOS_UNDEFINED,
    				SDL_WINDOWPOS_UNDEFINED,
    				constants::k_width,
    				constants::k_height,
    				SDL_WINDOW_SHOWN);

    sdlSetWindow(window);

	game::initializeGame();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}