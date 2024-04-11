#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Math/Math.h"
#include "Emulator/emulator.h"
#include <filesystem>

#define CITRA 0
#define MELON 1
#define STEAM 8


class Entity {
public:
	Entity(Vec2 posVec, SDL_Texture* Texture);

	Entity(Vec2 posVec, void* window, std::string Path, std::string RomPath, int gameType);

	virtual Vec2& getPos() { return pos; };

	virtual SDL_Texture* getTexture();
	virtual SDL_Rect getCurrentFrame();

	virtual bool isItemHovered(const int mouseX, const int mouseY);
	int isVisible(int y);

	void setPosition(Vec2 newPosition){pos = newPosition;}

	bool isHovered = false;

	Emulator emulator;
	std::string emulatorPath;
	std::string romPath;
	std::string gameName;

private:
	Vec2 pos;
	SDL_Rect currentFrame;
	SDL_Texture* texture;

	std::string getCitraAppCode(const std::string romPath);
	
	bool getCitraCover(std::string gameCode);
	void initCitra(void* window, const std::string& RomPath);

	bool getSteamCover(std::string gameCode);
	void initSteam(void* window, std::string& RomPath);

	void initMelon(void* window, const std::string& RomPath);
	bool getMelonCover(std::string& gameCode);
	std::string getDSAppCode(std::string romPath);
};


#define ScrollUp 1
#define ScrollDown 2
#define CloseEmulator 4
#define MaximizeWindow 8
#define AlwaysVisible 16

class systemButtons : public Entity {
public:
	systemButtons(Vec2 posVec, SDL_Texture* Texture,int Flags,int offset);

	Vec2& getPos() { return pos; };

	SDL_Texture* getTexture() { return texture; }
	SDL_Rect getCurrentFrame();

	bool isItemHovered(const int mouseX, const int mouseY);

	bool isVisible = false;
	int flags = 0;
	Vec2 pos;

	int xOffset;
	float yOffset;

private:
	SDL_Rect currentFrame;
	SDL_Texture* texture;
};
