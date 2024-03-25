#include "RenderWindow.h"
#include "Entities/Entity.h"
#include <format>
#include <OpenCV/opencv2/core/types_c.h>


RenderWindow::RenderWindow(const char* title, int x, int y)
	:window(nullptr),renderer(nullptr){

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x, y ,SDL_WINDOW_SHOWN);
	if (window == nullptr) std::cout << "Window failed to init.Error: " << SDL_GetError() << '\n';
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	backgroundPlayer.open("Background/menu.mp4");
	if (!backgroundPlayer.isOpened())
		std::cerr << "***Could not initialize capturing...***" << std::endl;


	backgroundPlayer >> mat;

	background = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, mat.cols,
		mat.rows);
	amountOfFrame = (int)backgroundPlayer.get(cv::CAP_PROP_FRAME_COUNT);
}

void RenderWindow::cleanUp() {
	SDL_DestroyWindow(window);
}

SDL_Texture* RenderWindow::loadTexture(const char* filePath){
	
	SDL_Texture* texture = nullptr;
	texture = IMG_LoadTexture(renderer, filePath);

	if (texture == nullptr) std::cout << "Failed to load texture. Error: " << SDL_GetError() << '\n';

	return texture;
}

void RenderWindow::clear() {
	SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity& entity) {

	SDL_Rect source;
	source.x = entity.getCurrentFrame().x;
	source.y = entity.getCurrentFrame().y;
	source.w = entity.getCurrentFrame().w;
	source.h = entity.getCurrentFrame().h;

	SDL_Rect destination;
	destination.x = entity.getPos().x;
	destination.y = entity.getPos().y;
	destination.w = entity.getCurrentFrame().w;
	destination.h = entity.getCurrentFrame().h;


	SDL_RenderCopy(renderer, entity.getTexture(), &source, &destination);
}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

int RenderWindow::getRefreshRate() {
	int displayIndex = SDL_GetWindowDisplayIndex(window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);

	return mode.refresh_rate;
}

void RenderWindow::renderBackground() {
	fillTexture();
	SDL_RenderCopy(renderer, background, NULL, NULL);
}

inline void RenderWindow::fillTexture(){
	frameCounter++;
	if (frameCounter >= amountOfFrame - 1) {
		backgroundPlayer.set(cv::CAP_PROP_POS_FRAMES, 0);
		frameCounter = 0;
	}

	backgroundPlayer >> mat;
	

	SDL_UpdateTexture(background, NULL, (void*)mat.data, mat.step1());

}