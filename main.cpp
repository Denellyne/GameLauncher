#include <SDL.h>
#include "Audio/audio.h"
#include "Renderer/RenderWindow.h"
#include "Renderer/TimeStepping.h"
#include "Entities/Entity.h"
#undef main

extern std::string AppName = "Bodfy Launcher";

int main() {
	bool isRunning = true;

	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048);

	RenderWindow window(AppName.c_str(), 1280, 720);

	Mix_Music* audio = loadSoundtrack("Audio/menu.mp3");
	Mix_PlayMusic(audio, -1);

	SDL_Event event;

	const float timeStep = 0.001f;
	float accumulator = 0;
	float currentTime = hireTimeInSeconds();

	float refreshRate = 1000.0f / window.getRefreshRate();
	int startTicks,frameTicks;
	float newTime, frameTime;
	std::vector<Entity> entities{ Entity(Vec2(100, 50), window.loadTexture("Graphics/citra.png"),"C:\\Users\\Gustavo Santos\\AppData\\Local\\Citra\\canary\\citra-qt.exe") };


	while (isRunning) {

		bool isLeftClickPressed = false;

		startTicks = SDL_GetTicks();

		newTime = hireTimeInSeconds();
		frameTime = newTime - currentTime;

		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= timeStep) {

			while (SDL_PollEvent(&event) != 0) {

				if (event.type == SDL_QUIT)
					isRunning = false;

				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) window.resizeEmulator();

				if (event.type == SDL_MOUSEBUTTONDOWN) isLeftClickPressed = true;
				
			}
			accumulator -= timeStep;
		}

		if (window.isEmulatorFocused()) {
			Mix_PauseMusic();
			window.clear();
			window.renderBackground();
			window.display();
		}
		else if (window.isWindowFocused()) {
			Mix_ResumeMusic();
			window.clear();
			window.renderBackground();
			
			//Render Entities
			window.getMousePosition();
			for(Entity& entity : entities) window.render(entity, isLeftClickPressed);

			window.display();
		}
		
		else Mix_PauseMusic();

		frameTicks = SDL_GetTicks() - startTicks;

		if (frameTicks < refreshRate) SDL_Delay(refreshRate - frameTicks);
	}

	window.cleanUp();
	Mix_FreeMusic(audio);
	SDL_Quit();
	return 0;
}











//if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) isRunning = false;
