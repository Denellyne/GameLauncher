#pragma once

#include <SDL.h>

inline float hireTimeInSeconds() {
	return SDL_GetTicks() * 0.001f;
}
