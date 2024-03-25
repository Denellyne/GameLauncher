#include <SDL.h>
#include "Audio/audio.h"
#include "Renderer/RenderWindow.h"
#include "Renderer/TimeStepping.h"
#include "Entities/Entity.h"

#undef main

int main() {

	bool isRunning = true;

	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048);

	RenderWindow window("Launcher", 1280, 720);

	Mix_Music* audio = loadSoundtrack("Audio/menu.mp3");
	Mix_PlayMusic(audio, -1);

	SDL_Event event;

	const float timeStep = 0.001f;
	float accumulator = 0;
	float currentTime = hireTimeInSeconds();

	float refreshRate = 1000.0f/60;
	int startTicks,frameTicks;
	float newTime, frameTime;
	
	while (isRunning) {

		startTicks = SDL_GetTicks();

		newTime = hireTimeInSeconds();
		frameTime = newTime - currentTime;

		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= timeStep) {

			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) isRunning = false;
			}
			accumulator -= timeStep;

		}

		window.clear();
		window.renderBackground();

		window.display();

		frameTicks = SDL_GetTicks() - startTicks;

		if (frameTicks < refreshRate) SDL_Delay(refreshRate - frameTicks);
	}

	window.cleanUp();
	Mix_FreeMusic(audio);
	SDL_Quit();
	return 0;
}