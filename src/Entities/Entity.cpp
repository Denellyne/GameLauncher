#include "Entity.h"

Entity::Entity(Vec2 posVec, SDL_Texture* Texture)
	:pos(posVec), texture(Texture) {
	
	currentFrame.x = 0;
	currentFrame.y = 0;
	currentFrame.w = 32;
	currentFrame.h = 32;
}


SDL_Texture* Entity::getTexture() {
	return texture;
}

SDL_Rect Entity::getCurrentFrame(){
	return currentFrame;
}

