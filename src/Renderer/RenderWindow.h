#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Entities/Entity.h"
#include "Entities/Emulator/emulator.h"
#include <mutex>
#include <fstream>
#include <OpenCV/highgui.hpp>

#define entityBelowBound 1
#define entityAboveBound 2

struct messageBox {
	SDL_Texture* texture;
	SDL_Rect Box;

};

class RenderWindow {

public:
	RenderWindow(const char* title,int x,int y);

	int getRefreshRate();

	bool isWindowFocused() { return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS); }
	void cleanUp();
	SDL_Texture* loadTexture(const char* filePath);
	void clear();
	int render(Entity& entity, bool isLeftClickPressed);
	void render(systemButtons& entity, bool isLeftClickPressed);
	void renderMessageBoxes();

	void renderBackground();
	void display();

	void startEmulator(std::string emulatorPath, std::string romPath);
	bool isEmulatorFocused();
	void resizeEmulator();
	void resizeEmulator(bool maximized);

	bool isEmulatorMaximized() { return emulator->maximized; }


	void getMousePosition() { SDL_GetMouseState(&mouseX, &mouseY); }
	void reorderHud(std::vector<systemButtons>& buttons, std::vector<Entity>& games);

	void scrollMenu(std::vector<Entity>& games);

	std::mutex lock;
	std::condition_variable threadWait;

private:
	SDL_Texture* background;
	SDL_Window* window;
	SDL_Renderer* renderer;

	inline void processNextFrame();
	int amountOfFrame, frameCounter = 0;

	cv::VideoCapture backgroundPlayer;
	cv::Mat mat;

	std::string appName;
	Emulator* emulator = nullptr;

	int mouseX, mouseY;

	short int scroll = 0;

	std::vector<messageBox> messageQueue;

};

