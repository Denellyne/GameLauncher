#include "RenderWindow.h"
#include "Entities/Entity.h"
#include <thread>
#include <format>



RenderWindow::RenderWindow(const char* title, int x, int y)
	:window(nullptr),renderer(nullptr){

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, x, y ,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) std::cout << "Window failed to init.Error: " << SDL_GetError() << '\n';
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


	cv::setNumThreads(1);
	cv::setUseOptimized(true);
	backgroundPlayer.open("Background/menu.mp4");
	if (!backgroundPlayer.isOpened())
		std::cerr << "***Could not initialize capturing...***" << std::endl;

	backgroundPlayer >> mat;

	background = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, mat.cols,
		mat.rows);
	amountOfFrame = (int)backgroundPlayer.get(cv::CAP_PROP_FRAME_COUNT);

	std::thread(&RenderWindow::processNextFrame,this).detach();
	appName = title;
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

void RenderWindow::render(Entity& entity,bool isLeftClickPressed) {

	entity.isItemHovered(mouseX,mouseY);
	if (entity.isHovered && isLeftClickPressed) {
		emulator = &entity.emulator;
		startEmulator(entity.path);
		resizeEmulator();
	}

	SDL_Rect source;
	source.x = entity.getCurrentFrame().x;
	source.y = entity.getCurrentFrame().y;
	source.w = entity.getCurrentFrame().w;
	source.h = entity.getCurrentFrame().h;

	SDL_Rect destination;
	destination.x = entity.getPos().x - entity.isHovered * 20;
	destination.y = entity.getPos().y - entity.isHovered * 20;
	destination.w = entity.getCurrentFrame().w + entity.isHovered * 40;
	destination.h = entity.getCurrentFrame().h + entity.isHovered * 40;


	SDL_RenderCopy(renderer, entity.getTexture(), &source, &destination);
}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

void RenderWindow::startEmulator(std::string path){
	emulator->initEmulator(path,appName);
}

bool RenderWindow::isEmulatorFocused(){

	extern std::string AppName;
	if (emulator == nullptr) return false;
	
	if (!GetWindow(FindWindowA(NULL, (LPCSTR)AppName.c_str()), GW_CHILD)) {
		emulator = nullptr;
		return false;
	}
	
	
	return true; 
}

int RenderWindow::getRefreshRate() {
	int displayIndex = SDL_GetWindowDisplayIndex(window);
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(displayIndex, 0, &mode);

	return mode.refresh_rate;
}

void RenderWindow::renderBackground() {

	std::unique_lock<std::mutex> ul(lock);
	SDL_UpdateTexture(background, NULL, (void*)mat.data, mat.step1());
	SDL_RenderCopy(renderer, background, NULL, NULL);
	threadWait.notify_one();

}

void RenderWindow::resizeEmulator() {

	if (emulator == nullptr) return;
	
	int x, y;
	SDL_GetWindowSize(window, &x, &y);
	emulator->resize(x, y);
}

inline void RenderWindow::processNextFrame() {

	const short limitFrameRate = (getRefreshRate()/ 60);

	while (true) {

		std::unique_lock<std::mutex> ul(lock);

		threadWait.wait(ul);
		threadWait.wait_for(ul,std::chrono::milliseconds(limitFrameRate));

		frameCounter++;

		if (frameCounter >= amountOfFrame - 3) {
			backgroundPlayer.set(cv::CAP_PROP_POS_FRAMES, 0);
			frameCounter = 0;
		}

		backgroundPlayer >> mat;
	}
}
