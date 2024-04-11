#pragma once
#include <Windows.h>
#include <string>

class Emulator {
public:

	Emulator(){}
	void initEmulator(const std::string& emulatorPath, const std::string& romPath, const std::string& appName);
	void closeEmulator();

	void resize(int x, int y);
	bool isFocused() { return GetForegroundWindow() == subWindowHandle; }

	~Emulator();
	HWND subWindowHandle = nullptr;
	bool maximized = false;

private:
	SHELLEXECUTEINFO processInfo;

};