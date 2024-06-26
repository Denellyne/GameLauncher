#include "main.h"
#undef SDL_main
extern std::string AppName = "Launcher";

inline static void getGames(std::vector<Entity>& games, RenderWindow& window);
static inline std::string getCustomEmulator(int dataType);

int main(int argc, char* argv[]) {
	SetProcessShutdownParameters(2, 0);
	SetConsoleCtrlHandler(exitFunction, TRUE);

	bool isRunning = true;
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048);

	RenderWindow window(AppName.c_str(), 1280, 720);
	exitPointer = &window;

	std::vector<Entity> games{};

	std::vector<systemButtons> buttons{
		systemButtons(Vec2(1280 - 16, 0), window.loadTexture("Graphics/System/Cross.png"), CloseEmulator, 16),
		systemButtons(Vec2(1280 - 32, 0), window.loadTexture("Graphics/System/MaximizeWindow.png"), MaximizeWindow, 32),
		systemButtons(Vec2(1280 - 32, 50), window.loadTexture("Graphics/System/Up.png"), ScrollDown, 32),
		systemButtons(Vec2(1280 - 32, 690), window.loadTexture("Graphics/System/Down.png"), ScrollUp, 32)
	};

	Mix_Music* audio = loadSoundtrack("Audio/menu.mp3");
	Mix_PlayMusic(audio, -1);

	SDL_Event event;

	const float timeStep = 0.001f;
	float accumulator = 0;
	float currentTime = hireTimeInSeconds();

	float refreshRate = 1000.0f / window.getRefreshRate();
	int startTicks,frameTicks;
	float newTime, frameTime;

	//https://art.gametdb.com/ds/coverM/US/GameCode.jpg
	getGames(games, window);
	
	while (isRunning) {
		bool isLeftClickPressed = false;

		startTicks = SDL_GetTicks();

		newTime = hireTimeInSeconds();
		frameTime = newTime - currentTime;

		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= timeStep) {

			while (SDL_PollEvent(&event) != 0) {

				if (event.type == SDL_QUIT)
					isRunning = false;

				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
					window.resizeEmulator();
					window.reorderHud(buttons,games);
				}

				if (event.type == SDL_MOUSEBUTTONDOWN) isLeftClickPressed = true;
				
			}
			accumulator -= timeStep;
		}

		if (window.isEmulatorFocused()) {
			Mix_PauseMusic();
			window.clear();
			if(!window.isEmulatorMaximized())
				window.renderBackground();

			//Render Buttons
			window.getMousePosition();
			for (systemButtons& entity : buttons) window.render(entity, isLeftClickPressed);

			window.display();
		}
		else if (window.isWindowFocused()) {
			Mix_ResumeMusic();
			window.clear();
			window.renderBackground();
			
			//Render Entities
			window.getMousePosition();
			for (int i = 0; i < games.size(); i++) {
				
				int code = window.render(games[i], isLeftClickPressed);
				switch (code) {
				case entityBelowBound:
					i = games.size();
					break;
				case entityAboveBound: break;
				default: break;

				}
			}
			for (systemButtons& entity : buttons) window.render(entity, isLeftClickPressed);
			window.renderMessageBoxes();
			window.scrollMenu(games);

			window.display();
		}
		else Mix_PauseMusic();

		frameTicks = SDL_GetTicks() - startTicks;

		if (frameTicks < refreshRate) SDL_Delay(refreshRate - frameTicks);
	}

	Mix_FreeMusic(audio);
	Mix_CloseAudio();
	return 0;
}

static inline std::string getCustomEmulator(int dataType) {
	char filename[MAX_PATH];
	
	OPENFILENAME file;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&file, sizeof(file));
	file.lStructSize = sizeof(file);
	file.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	switch (dataType) {
	case 1:
		file.lpstrFilter = _T("Executable\0*.exe\0");
		file.lpstrTitle = _T("Select the corresponding emulator");
		break;
	case 2:
		file.lpstrFilter = _T("Images\0*.png;*.jpg\0");
		file.lpstrTitle = _T("Select the corresponding emulator");
		
	}

	file.lpstrFile = (LPWSTR)filename;
	file.nMaxFile = MAX_PATH;
	file.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (!GetOpenFileName(&file)) return "NULL";
	std::string path;
	for (int i = 0; file.lpstrFile[i] != '\0'; i++) path += file.lpstrFile[i];

	return path;
}

inline static void getGames(std::vector<Entity>& games, RenderWindow& window) {
	namespace fs = std::filesystem;
#define arraySize(x) sizeof(x)/sizeof(x[0])

	int x = 30;
	int y = 50;
	const std::tuple<std::string, std::string, int>romExtensions[]{ { ".3ds", "C:\\Users\\Gustavo Santos\\AppData\\Local\\Citra\\canary\\citra-qt.exe",CITRA} ,{".nds","\"C:\\Users\\Gustavo Santos\\Downloads\\c\\melonDS.exe\"",MELON } };

	for (const auto& dirEntry : fs::recursive_directory_iterator("./Roms", fs::directory_options::skip_permission_denied)) {
		if (dirEntry.is_directory()) continue;
		try {
			for (const std::tuple<std::string, std::string, int>& romExtension : romExtensions) {
				if (!dirEntry.path().string().ends_with(std::get<0>(romExtension))) continue;
				if (x + 100 > 1280) {
					y += 140;
					x = 30;
				}
				Entity newEntity(Vec2(x, y), &window, std::get<1>(romExtension), dirEntry.path().string(), std::get<2>(romExtension));
				games.push_back(newEntity);

				x += 100;
			}

		}
		catch (...) { ; }
	}

	//Get Steam Games
#define steamworks "228980"
	if (fs::exists("C:\\Program Files (x86)\\Steam\\steamapps") == false) return;

	for (const auto& dirEntry : fs::directory_iterator("C:\\Program Files (x86)\\Steam\\steamapps\\", fs::directory_options::skip_permission_denied)) {
		if (dirEntry.is_directory() || dirEntry.path().string().ends_with("libraryfolders.vdf")) continue;
		try {
			std::string appId = dirEntry.path().string();
			appId = appId.substr(appId.find_first_of("_") + 1);
			appId = appId.substr(0, appId.find_first_of("."));
			if (appId == steamworks) continue;

			if (x + 100 > 1280) {
				y += 140;
				x = 30;
			}
			Entity newEntity(Vec2(x, y), &window, "cmd", dirEntry.path().string(), STEAM);
			games.push_back(newEntity);

			x += 100;

		}
		catch (...) { ; }
	}


}



//if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) isRunning = false;
