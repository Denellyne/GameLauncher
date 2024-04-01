#pragma once
#include <Windows.h>
#include <string>

class Emulator {
public:

	Emulator(){}
	void initEmulator(const std::string& path, const std::string& appName);
	void closeEmulator();

	void resize(int x, int y);
	bool isFocused() { return GetForegroundWindow() == subWindowHandle; }

	~Emulator();
	HWND subWindowHandle;

private:
	SHELLEXECUTEINFO processInfo;

};