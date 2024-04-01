#include "Entity.h"

Entity::Entity(Vec2 posVec, SDL_Texture* Texture,std::string Path)
	:pos(posVec), texture(Texture),path(Path) {
	
	currentFrame.x = 0;
	currentFrame.y = 0;
	currentFrame.w = 80;
	currentFrame.h = 128;
}


SDL_Texture* Entity::getTexture() {
	return texture;
}

SDL_Rect Entity::getCurrentFrame(){
	return currentFrame;
}

void Entity::isItemHovered(const int mouseX, const int mouseY){


	if (pos.x < mouseX &&
		pos.x + currentFrame.w > mouseX &&
		pos.y < mouseY &&
		pos.y + currentFrame.h > mouseY) {
		
		isHovered = true;
		return;
	}


	isHovered = false;
}

