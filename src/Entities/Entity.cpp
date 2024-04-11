#include "Entity.h"
#include <OpenCV/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <format>
#include <filesystem>
#include <fstream>
#include "Renderer/RenderWindow.h"

Entity::Entity(Vec2 posVec, void* window, std::string Path,std::string RomPath,int gameType)
	:pos(posVec),emulatorPath(Path),romPath(std::format("\"{}\"", RomPath)){
	
	currentFrame.x = 0;
	currentFrame.y = 0;
	currentFrame.w = 80;
	currentFrame.h = 128;

	switch (gameType) {
	case CITRA: initCitra(window, RomPath); break;
	case MELON: initMelon(window, RomPath); break;
	case STEAM: initSteam(window, RomPath); break;
	default:	
		gameName = RomPath.substr(RomPath.find_last_of("\\") + 1);
		gameName = gameName.substr(0, gameName.find_first_of("."));
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture("Graphics/Roms/CitraGeneric.png");
		break;
	}
	
}

Entity::Entity(Vec2 posVec, SDL_Texture* Texture)
		:pos(posVec), texture(Texture) {

		currentFrame.x = 0;
		currentFrame.y = 0;
		currentFrame.w = 16;
		currentFrame.h = 16;
	
}

SDL_Texture* Entity::getTexture() {
	return texture;
}

SDL_Rect Entity::getCurrentFrame(){
	return currentFrame;
}

bool Entity::isItemHovered(const int mouseX, const int mouseY){


	if (pos.x < mouseX &&
		pos.x + currentFrame.w > mouseX &&
		pos.y < mouseY &&
		pos.y + currentFrame.h > mouseY) {
		
		isHovered = true;
		return true;
	}


	isHovered = false;
	return false;
}

int Entity::isVisible(int y){

	if (pos.y >= y) return entityBelowBound;
	if(pos.y + currentFrame.h < 0) return entityAboveBound;

	return 0;
}

std::string Entity::getCitraAppCode(const std::string romPath) {
	#define citraAppCodeOffset 276

	std::ifstream get3DSCode(romPath, std::fstream::binary);
	short int i = 0;
	char buffer[16];
	while (get3DSCode.good()) {
		if (i > citraAppCodeOffset) {
			get3DSCode.read(buffer, 6);
			get3DSCode.read(buffer, 6);
			get3DSCode.close();
			return buffer;
		}

		get3DSCode.read(buffer, 16);
		i++;
	}
	get3DSCode.close();
	return "ERROR";
}

bool Entity::getCitraCover(std::string gameCode) {
	system(std::format("curl https://art.gametdb.com/3ds/coverHQ/EN/{}.jpg -o Graphics/Roms/Temp.png -L", gameCode).c_str());
	
	if (std::filesystem::file_size("Graphics/Roms/Temp.png") == 0) system(std::format("curl https://art.gametdb.com/3ds/coverHQ/US/{}.jpg -o Graphics/Roms/Temp.png -L", gameCode).c_str());

	if (std::filesystem::file_size("Graphics/Roms/Temp.png") == 0) {
		std::filesystem::remove("Graphics/Roms/Temp.png");
		return false;
	}

	cv::Mat citraGeneric = cv::imread(cv::samples::findFile("Graphics/Roms/CitraBase.png"));
	cv::Mat boxArt = cv::imread(cv::samples::findFile("Graphics/Roms/Temp.png"));

	cv::Rect roi(0, 0, 685, 650);
	cv::Mat croppedBoxArt(boxArt, roi);
	cv::resize(croppedBoxArt, croppedBoxArt, cv::Size(), 0.09, 0.165);

	croppedBoxArt.copyTo(citraGeneric(cv::Rect((citraGeneric.cols - croppedBoxArt.cols) / 2, 3, croppedBoxArt.cols, croppedBoxArt.rows)));

	cv::imwrite(std::format("Graphics/Roms/{}.png", gameCode), citraGeneric);
	std::filesystem::remove("Graphics/Roms/Temp.png");
	return true;
}

bool Entity::getSteamCover(std::string gameCode) {
	system(std::format("curl https://steamcdn-a.akamaihd.net/steam/apps/{}/library_600x900_2x.jpg -o Graphics/Roms/Temp.png -L", gameCode).c_str());

	cv::Mat steamGeneric = cv::imread(cv::samples::findFile("Graphics/Roms/SteamBase.png"));
	cv::Mat boxArt = cv::imread(cv::samples::findFile("Graphics/Roms/Temp.png"));

	if (boxArt.empty()) {
		std::filesystem::remove("Graphics/Roms/Temp.png");
		return false;
	}

	cv::resize(boxArt, boxArt, cv::Size(), 0.12, 0.12);

	boxArt.copyTo(steamGeneric(cv::Rect((steamGeneric.cols - boxArt.cols) / 2, 3, boxArt.cols, boxArt.rows)));

	cv::imwrite(std::format("Graphics/Roms/{}.png", gameCode), steamGeneric);
	std::filesystem::remove("Graphics/Roms/Temp.png");
	return true;
}



void Entity::initCitra(void* window, const std::string& RomPath) {
	std::string gameCode = getCitraAppCode(RomPath);
	gameName = RomPath.substr(RomPath.find_last_of("\\") + 1);
	gameName = gameName.substr(0, gameName.find_first_of("."));


	if (gameCode == "ERROR") {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture("Graphics/Roms/CitraGeneric.png");
		return;
	}
	if (std::filesystem::exists(std::format("Graphics/Roms/{}.png", gameCode))) {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture(std::format("Graphics/Roms/{}.png", gameCode).c_str());
		return;
	}


	if (!getCitraCover(gameCode)) {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture("Graphics/Roms/CitraGeneric.png");
		return;
	}

	texture = reinterpret_cast<RenderWindow*>(window)->loadTexture(std::format("Graphics/Roms/{}.png", gameCode).c_str());
}

void Entity::initSteam(void* window, std::string& RomPath) {
	std::string temp = RomPath;

	std::ifstream getName(RomPath);
	while (getName.good()) {
		getName.getline(&temp[0], 256);
		if (strstr(temp.c_str(), "name")) {
			temp = temp.substr(temp.find_first_of("\"", 7) + 1);
			temp = temp.substr(0, temp.find_first_of("\""));
			gameName = temp;
			break;
		}
	}
	getName.close();

	RomPath = RomPath.substr(RomPath.find_first_of("_") + 1);
	RomPath = RomPath.substr(0, RomPath.find_first_of("."));

	romPath = std::format("/C start steam://launch/{}/", RomPath);
	if (std::filesystem::exists(std::format("Graphics/Roms/{}.png", RomPath))) {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture(std::format("Graphics/Roms/{}.png", RomPath).c_str());
		return;
	}
	if (!getSteamCover(RomPath)) {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture("Graphics/Roms/SteamGeneric.png");
		return;
	}
	texture = reinterpret_cast<RenderWindow*>(window)->loadTexture(std::format("Graphics/Roms/{}.png", RomPath).c_str());
}

std::string Entity::getDSAppCode(std::string romPath) {
	std::ifstream getDSCode(romPath, std::fstream::binary);
	char buffer[16];
	if (getDSCode.good()) {
		getDSCode.read(buffer, 12);
		memset(buffer, 0, 16);
		getDSCode.read(buffer, 4);
		getDSCode.close();
		return buffer;

	}
	getDSCode.close();
	return "ERROR";
}

bool Entity::getMelonCover(std::string& gameCode) {


	system(std::format("curl https://art.gametdb.com/ds/coverM/US/{}.jpg -o Graphics/Roms/Temp.png -L", gameCode).c_str());

	if (std::filesystem::file_size("Graphics/Roms/Temp.png") == 0) system(std::format("curl https://art.gametdb.com/ds/coverM/EN/{}.jpg -o Graphics/Roms/Temp.png -L", gameCode).c_str());

	if (std::filesystem::file_size("Graphics/Roms/Temp.png") == 0) {
		//std::filesystem::remove("Graphics/Roms/Temp.png");
		return false;
	}

	cv::Mat citraGeneric = cv::imread(cv::samples::findFile("Graphics/Roms/MelonBase.png"));
	cv::Mat boxArt = cv::imread(cv::samples::findFile("Graphics/Roms/Temp.png"));

	cv::Rect roi(60, 0,340, 352);
	cv::Mat croppedBoxArt(boxArt, roi);
	cv::resize(croppedBoxArt, croppedBoxArt, cv::Size(), 0.21, 0.307);

	croppedBoxArt.copyTo(citraGeneric(cv::Rect((citraGeneric.cols - croppedBoxArt.cols) / 2, 3, croppedBoxArt.cols, croppedBoxArt.rows)));

	cv::imwrite(std::format("Graphics/Roms/{}.png", gameCode), citraGeneric);
//	std::filesystem::remove("Graphics/Roms/Temp.png");
	return true;
}


void Entity::initMelon(void* window, const std::string& RomPath) {
	std::string gameCode = getDSAppCode(RomPath);

	gameName = RomPath.substr(RomPath.find_last_of("\\") + 1);
	gameName = gameName.substr(0, gameName.find_first_of("."));


	if (gameCode == "ERROR") {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture("Graphics/Roms/MelonGeneric.png");
		return;
	}
	if (std::filesystem::exists(std::format("Graphics/Roms/{}.png", gameCode))) {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture(std::format("Graphics/Roms/{}.png", gameCode).c_str());
		return;
	}


	if (!getMelonCover(gameCode)) {
		texture = reinterpret_cast<RenderWindow*>(window)->loadTexture("Graphics/Roms/MelonGeneric.png");
		return;
	}

	texture = reinterpret_cast<RenderWindow*>(window)->loadTexture(std::format("Graphics/Roms/{}.png", gameCode).c_str());
}











systemButtons::systemButtons(Vec2 posVec, SDL_Texture* Texture,int Flags,int offset)
	:Entity(posVec,Texture),flags(Flags),xOffset(offset) {


	pos = posVec;
	yOffset = pos.y / 720.0f;
	texture = Texture;
	currentFrame.x = 0;
	currentFrame.y = 0;
	currentFrame.w = 16;
	currentFrame.h = 16;

	if (Flags & AlwaysVisible) isVisible = true;

}

SDL_Rect systemButtons::getCurrentFrame() {
	return currentFrame;
}

bool systemButtons::isItemHovered(const int mouseX, const int mouseY) {


	if (pos.x < mouseX &&
		pos.x + currentFrame.w > mouseX &&
		pos.y < mouseY &&
		pos.y + currentFrame.h > mouseY) {

		isHovered = true;
		return true;
	}

	isHovered = false;
	return false;
}

