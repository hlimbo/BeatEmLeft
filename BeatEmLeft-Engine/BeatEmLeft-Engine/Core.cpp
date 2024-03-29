#include "Core.h"
#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define SUCCESS 0

//TODO: refactor Core into a struct and remove all getters from it
//app initialization
Core::Core()
{
	if (!appInitialization())
		printf("App Initialization not successful\n");
}

//app destruction
Core::~Core()
{
	appCleanUp();
}

Uint64 Core::getTargetFPS()
{
	return targetFPS;
}

float Core::getTargetDeltaTime()
{
	return targetDeltaTime;
}

SDL_Renderer* Core::getRenderer()
{
	return mainRenderer;
}

SDL_Window* Core::getWindow()
{
	return mainWindow;
}

bool Core::appInitialization()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	if (SDL_GetNumVideoDisplays() <= 0)
	{
		printf("No display monitors attached to current device: %s\n", SDL_GetError());
		return false;
	}

	//TODO: move to GameLoop struct constructor
	//obtain target fps based on monitor's refresh rate (hz).
	SDL_DisplayMode displayMode;
	//get the first monitor attached to device display mode settings.
	if (SDL_GetDisplayMode(0, 0, &displayMode) != SUCCESS)
	{
		printf("Unable to get display mode settings : %s\n", SDL_GetError());
		return false;
	}

	//TODO: move to GameLoop struct constructor
	//cap targetFPS to 60
	targetFPS = displayMode.refresh_rate > 60 ? 60 : displayMode.refresh_rate;
	targetDeltaTime = 1000.0f / targetFPS;

	mainWindow = SDL_CreateWindow("Beat Em Left", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (mainWindow == NULL)
	{
		printf("Main window creation failed : %s\n", SDL_GetError());
		return false;
	}

	mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (mainRenderer == NULL)
	{
		printf("Main renderer creation failed : %s\n", SDL_GetError());
		return false;
	}

	//load external sdl libraries
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) == 0)
	{
		printf("Failed to initialize SDL IMG Library: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() == -1)
	{
		printf("Failed to initialize SDL TTF Library: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

void Core::appCleanUp()
{
	IMG_Quit();
	if (TTF_WasInit())
		TTF_Quit();
	if (mainRenderer != NULL)
	{
		SDL_DestroyRenderer(mainRenderer);
		mainRenderer = NULL;
	}
	if (mainWindow != NULL)
	{
		SDL_DestroyWindow(mainWindow);
		mainWindow = NULL;
	}
	SDL_Quit();
}




