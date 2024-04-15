#pragma once
#include <SDL.h>
#include "Audio/audio.h"
#include "Renderer/RenderWindow.h"
#include "Renderer/TimeStepping.h"
#include "Entities/Entity.h"
#include <tchar.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>


RenderWindow* exitPointer = nullptr;

extern BOOL WINAPI exitFunction(_In_ DWORD dwCtrlType) {
	
	exitPointer->~RenderWindow();

	return TRUE;
}
