#include "emulator.h"
#include <Windows.h>
#include <string>
#include <iostream>

struct processData {
	unsigned long processId;
	HWND windowHandle;
};

BOOL isMainWindow(HWND handle) {
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enumWindows(HWND handle, LPARAM lParam) {
	processData& data = *(processData*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.processId != process_id || !isMainWindow(handle)) 
		return TRUE;
	
	data.windowHandle = handle;

	return FALSE;
}

HWND getWindowHandle(HANDLE processHandle) {
	processData data;
	data.processId = GetProcessId(processHandle);
	data.windowHandle = 0;
    #define PASSES 500

	for (short int i = 0; i < PASSES; i++) { //Checks if app has opened yet
		EnumWindows(enumWindows, (LPARAM)&data);
		if (data.windowHandle != NULL) break;
		Sleep(20);
	}
	
	return data.windowHandle;
}


void Emulator::initEmulator(const std::string& emulatorPath, const std::string& romPath, const std::string& appName){
	

	std::wstring tempEmulatorPath = std::wstring(emulatorPath.begin(), emulatorPath.end()).c_str();
	LPCWSTR convertedEmulatorPath = tempEmulatorPath.c_str();

	std::wstring tempRomPath = std::wstring(romPath.begin(), romPath.end()).c_str();
	LPCWSTR convertedRomPath = tempRomPath.c_str();

	processInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	processInfo.lpFile = convertedEmulatorPath;
	processInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	processInfo.hwnd = NULL;
	processInfo.lpVerb = NULL;
	processInfo.lpParameters = convertedRomPath;
	processInfo.lpDirectory = NULL;
	processInfo.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&processInfo);

	subWindowHandle = getWindowHandle(processInfo.hProcess);

	if (subWindowHandle == 0 && emulatorPath != "cmd") {
		TerminateProcess(processInfo.hProcess, 0);
		return;
	}

	LONG lStyle = GetWindowLong(subWindowHandle, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_VSCROLL | WS_HSCROLL | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_DISABLED | WS_CHILD);
	SetWindowLongPtr(subWindowHandle, GWL_STYLE, lStyle);

	SetParent(subWindowHandle, FindWindowA(NULL, (LPCSTR)appName.c_str()));
	SetWindowPos(subWindowHandle, NULL, 40, 85, 1200, 550, 0);

}

void Emulator::closeEmulator(){
	if (subWindowHandle == nullptr) return;
	TerminateProcess(processInfo.hProcess, 0);
	subWindowHandle = NULL;
}

Emulator::~Emulator() {
	TerminateProcess(processInfo.hProcess, 0);
}

void Emulator::resize(int x ,int y){
	if (maximized) {
		SetWindowPos(subWindowHandle, NULL, 0, 16, x, y, 0);
		return;
	}

	SetWindowPos(subWindowHandle, NULL, 40, 85, x - 80, y - 170, 0);
}
