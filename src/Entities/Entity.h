#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Math/Math.h"
#include "Emulator/emulator.h"

class Entity {

public:
	Entity(Vec2 posVec,SDL_Texture* texture,std::string Path);
	virtual Vec2& getPos() { return pos; };

	SDL_Texture* getTexture();
	virtual SDL_Rect getCurrentFrame();

	void isItemHovered(const int mouseX, const int mouseY);


	bool isHovered = false;
	Emulator emulator;
	std::string path;


private:
	Vec2 pos;
	SDL_Rect currentFrame;
	SDL_Texture* texture;


};

