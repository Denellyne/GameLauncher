#include <SDL.h>
#include "Audio/audio.h"
#include "Renderer/RenderWindow.h"
#include "Renderer/TimeStepping.h"
#include "Entities/Entity.h"
#include <Windows.h>
#include <sstream>
#undef main

extern int a, b;
#define AppName "Launcher"

int main() {

	bool isRunning = true;

	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048);

	RenderWindow window(AppName, 1280, 720);

	Mix_Music* audio = loadSoundtrack("Audio/menu.mp3");
	Mix_PlayMusic(audio, -1);

	SDL_Event event;

	const float timeStep = 0.001f;
	float accumulator = 0;
	float currentTime = hireTimeInSeconds();

	float refreshRate = 1000.0f / window.getRefreshRate();
	int startTicks,frameTicks;
	float newTime, frameTime;

	SHELLEXECUTEINFO lpExecInfo{};
	lpExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	lpExecInfo.lpFile = L"\"C:\\Users\\Gustavo Santos\\AppData\\Local\\Citra\\canary\\citra-qt.exe\"";
	lpExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	lpExecInfo.hwnd = NULL;
	lpExecInfo.lpVerb = NULL;
	lpExecInfo.lpParameters = NULL;
	lpExecInfo.lpDirectory = NULL;
	lpExecInfo.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&lpExecInfo);

	Sleep(2500);
	HWND subWindowHandle = FindWindowA(NULL, "Citra Canary 2782");
	SDL_Window*  subWindow = SDL_CreateWindowFrom((void*)subWindowHandle);

	LONG lStyle = GetWindowLong(subWindowHandle, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_VSCROLL | WS_HSCROLL | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_DISABLED | WS_CHILD);
	SetWindowLongPtr(subWindowHandle, GWL_STYLE, lStyle);

	SetParent(subWindowHandle, FindWindowA(NULL, AppName));
	SetWindowPos(subWindowHandle, NULL, 40, 85, 1200, 550, 0);

	std::cerr << SDL_GetError() << '\n';
	SDL_Renderer* subRenderer = SDL_CreateRenderer(subWindow, -1, SDL_RENDERER_ACCELERATED);
	

	while (isRunning) {

		startTicks = SDL_GetTicks();

		newTime = hireTimeInSeconds();
		frameTime = newTime - currentTime;

		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= timeStep) {

			while (SDL_PollEvent(&event) != 0) {
				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) isRunning = false;

				if (event.type == SDL_QUIT)
					isRunning = false;
				
			}
			accumulator -= timeStep;
		}

		if (window.isWindowFocuses()) {

			Mix_ResumeMusic();
			window.clear();
			window.renderBackground();
			window.display();
		}
		else if (GetForegroundWindow() == subWindowHandle) {
			Mix_PauseMusic();
			window.clear();
			window.renderBackground();
			window.display();
		}
		else Mix_PauseMusic();

		frameTicks = SDL_GetTicks() - startTicks;

		if (frameTicks < refreshRate) SDL_Delay(refreshRate - frameTicks);
	}

	TerminateProcess(lpExecInfo.hProcess, 0);
	CloseHandle(lpExecInfo.hProcess);
	SDL_DestroyWindow(subWindow);
	SDL_DestroyRenderer(subRenderer);
	window.cleanUp();
	Mix_FreeMusic(audio);
	SDL_Quit();
	return 0;
}