#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Entities/Entity.h"
#include <OpenCV/highgui.hpp>

class RenderWindow {

public:
	RenderWindow(const char* title,int x,int y);

	int getRefreshRate();

	void cleanUp();
	SDL_Texture* loadTexture(const char* filePath);
	void clear();
	void render(Entity& entity);

	void renderBackground();
	void display();

private:
	inline void fillTexture();

	SDL_Texture* background;
	SDL_Window* window;
	SDL_Renderer* renderer;
	cv::VideoCapture backgroundPlayer;
	cv::Mat mat;
	int amountOfFrame,frameCounter=0;


};

