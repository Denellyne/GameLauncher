#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Math/Math.h"

class Entity {

public:
	Entity(Vec2 posVec,SDL_Texture* texture);
	virtual Vec2& getPos() { return pos; };
	virtual SDL_RendererFlip getRotation() { return rotation; };

	SDL_Texture* getTexture();
	virtual SDL_Rect getCurrentFrame();

private:
	Vec2 pos;
	SDL_Rect currentFrame;
	SDL_Texture* texture;
	SDL_RendererFlip rotation = SDL_FLIP_NONE;

};

