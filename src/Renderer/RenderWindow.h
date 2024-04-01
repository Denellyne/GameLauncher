#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Entities/Entity.h"
#include "Entities/Emulator/emulator.h"
#include <mutex>
#include <OpenCV/highgui.hpp>

class RenderWindow {

public:
	RenderWindow(const char* title,int x,int y);

	int getRefreshRate();

	bool isWindowFocused() { return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS); }
	void cleanUp();
	SDL_Texture* loadTexture(const char* filePath);
	void clear();
	void render(Entity& entity, bool isLeftClickPressed);

	void renderBackground();
	void display();

	void startEmulator(std::string path);
	bool isEmulatorFocused();
	void resizeEmulator();
	void getMousePosition() { SDL_GetMouseState(&mouseX, &mouseY); }



private:
	SDL_Texture* background;
	SDL_Window* window;
	SDL_Renderer* renderer;

	inline void processNextFrame();
	std::condition_variable threadWait;
	std::mutex lock;
	int amountOfFrame, frameCounter = 0;

	cv::VideoCapture backgroundPlayer;
	cv::Mat mat;

	std::string appName;
	Emulator* emulator;

	int mouseX, mouseY;
};

