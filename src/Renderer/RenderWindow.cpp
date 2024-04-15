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

RenderWindow::~RenderWindow() {
	TTF_Quit();
	cleanUp();
	SDL_Quit();


	if (emulator == nullptr) return;
	emulator->closeEmulator();
}


void RenderWindow::cleanUp() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
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

int RenderWindow::render(Entity& entity,bool isLeftClickPressed) {
	int y,code;
	SDL_GetWindowSize(window, NULL, &y);
	code = entity.isVisible(y);
	if (code != 0) return code;
	entity.isItemHovered(mouseX, mouseY);
	
	if (entity.isHovered && isLeftClickPressed) {
		emulator = &entity.emulator;
		startEmulator(entity.emulatorPath,entity.romPath);
		resizeEmulator();
	}

	SDL_Rect source;
	source.x = entity.getCurrentFrame().x;
	source.y = entity.getCurrentFrame().y;
	source.w = entity.getCurrentFrame().w;
	source.h = entity.getCurrentFrame().h;

	SDL_Rect destination;
	destination.x = entity.getPos().x - entity.isHovered * 18;
	destination.y = entity.getPos().y - entity.isHovered * 18;
	destination.w = entity.getCurrentFrame().w + entity.isHovered * 36;
	destination.h = entity.getCurrentFrame().h + entity.isHovered * 36;

	SDL_RenderCopy(renderer, entity.getTexture(), &source, &destination);
	
	if (entity.isHovered) {

		TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 18);

		SDL_Color White = { 255, 255, 255 };

		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, entity.gameName.c_str(), White);

		TTF_CloseFont(font);

		messageBox newMessage;

		newMessage.texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

		SDL_FreeSurface(surfaceMessage);

		SDL_GetWindowSize(window, &newMessage.Box.x, NULL);
		newMessage.Box.x *= 0.5;
		newMessage.Box.x -= entity.gameName.length()*7.5f;
		newMessage.Box.y = 0;
		newMessage.Box.w = entity.gameName.length()*15;
		newMessage.Box.h = 40;

		messageQueue.push_back(newMessage); 
	}
	return code;

}

void RenderWindow::render(systemButtons& entity, bool isLeftClickPressed){
	
	bool isHovered = entity.isItemHovered(mouseX, mouseY);

	entity.isVisible = (isHovered || (entity.flags & AlwaysVisible) || (((entity.flags & ScrollUp || entity.flags & ScrollDown) && emulator == nullptr)));

	if (entity.isVisible == false || (entity.flags & CloseEmulator && emulator == nullptr) || (entity.flags & MaximizeWindow && emulator == nullptr)
		|| (entity.flags & ScrollUp && emulator != nullptr) || (entity.flags & ScrollDown && emulator != nullptr)) return;

	if (isHovered && isLeftClickPressed) {

		if (entity.flags & CloseEmulator && emulator != nullptr) {
			emulator->closeEmulator();
			emulator = nullptr;
		}
		if (entity.flags & MaximizeWindow && emulator != nullptr) 
			resizeEmulator(emulator->maximized);
		
		if (entity.flags & ScrollUp) 
			scroll = ScrollUp;
		
		if (entity.flags & ScrollDown) 
			scroll = ScrollDown;
		
	}

	SDL_Rect source;
	source.x = entity.getCurrentFrame().x;
	source.y = entity.getCurrentFrame().y;
	source.w = entity.getCurrentFrame().w;
	source.h = entity.getCurrentFrame().h;

	SDL_Rect destination;
	destination.x = (entity.getPos().x) * entity.isVisible;
	destination.y = (entity.getPos().y) * entity.isVisible;
	destination.w = (entity.getCurrentFrame().w) * entity.isVisible;
	destination.h = (entity.getCurrentFrame().h) * entity.isVisible;

	SDL_RenderCopy(renderer, entity.getTexture(), &source, &destination);

}

void RenderWindow::renderMessageBoxes(){

	while (messageQueue.size() != 0) {


		SDL_Rect messageBorder = messageQueue[0].Box;
		messageBorder.h += 3;
		messageBorder.w += 5;
		messageBorder.x -= 3;

		SDL_SetRenderDrawColor(renderer, 34, 32, 52, 255);
		SDL_RenderFillRect(renderer, &messageBorder);
		SDL_SetRenderDrawColor(renderer, 35, 26, 39, 255);
		SDL_RenderFillRect(renderer, &messageQueue[0].Box);
		SDL_RenderCopy(renderer, messageQueue[0].texture, NULL, &messageQueue[0].Box);

		SDL_DestroyTexture(messageQueue[0].texture);
		messageQueue.erase(messageQueue.begin());
	}

}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

void RenderWindow::startEmulator(std::string emulatorPath,std::string romPath){
	emulator->initEmulator(emulatorPath,romPath,appName);
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

void RenderWindow::reorderHud(std::vector<systemButtons>& buttons,std::vector<Entity>& games) {
	
	int windowX,windowY;
	
	SDL_GetWindowSize(window, &windowX, &windowY);

	for (int i = 0; i < buttons.size(); i++) {
		buttons[i].pos.x = windowX - buttons[i].xOffset;
		buttons[i].pos.y = windowY * buttons[i].yOffset;
	}

	if (games.size() == 0) return;
	int x = 30, y = games[0].getPos().y;

	for (Entity& entity : games) {
		if (x + 100 > windowX) {
			y += 140;
			x = 30;
		}
		entity.setPosition({ (float)x,(float)y });

		x += 100;
	}

}

void RenderWindow::resizeEmulator() {

	if (emulator == nullptr) return;
	
	int x, y;
	SDL_GetWindowSize(window, &x, &y);
	emulator->resize(x, y);
}

void RenderWindow::resizeEmulator(bool maximized) {

	int x, y;
	SDL_GetWindowSize(window, &x, &y);
	emulator->maximized = !emulator->maximized;
	
	emulator->resize(x, y);
}

void RenderWindow::scrollMenu(std::vector<Entity>& games){
	if (scroll == 0) return;

	if(scroll == ScrollUp)
		for (Entity& entity : games)
			entity.setPosition({ entity.getPos().x ,entity.getPos().y - 140 });
	else 
		for (Entity& entity : games)
			entity.setPosition({ entity.getPos().x ,entity.getPos().y + 140 });

	scroll = 0;
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
