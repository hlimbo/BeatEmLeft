#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 480

//global variables for demo purposes
SDL_Window* gWindow = NULL;
//The surface contained inside the window
SDL_Surface* gScreenSurface = NULL;
//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

//Starts up SDL and creates window
bool init();
bool loadMedia();
//Frees media and shuts down SDL
void close();

int main(int argc, char* argv[])
{
	if (!init())
		puts("Failed to initialize");
	else
	{
		if (!loadMedia())
		{
			puts("Failed to load media!");
		}
		else
		{
			//SDL is doubled buffered blitsurface draws to the back buffer.
			//Apply the image - renders to the back buffer
			SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
			
			//put what was drawn on the backbuffer to the front buffer.
			//Call this only when AFTER all the blits for the current frame are done.
			SDL_UpdateWindowSurface(gWindow);

			SDL_Delay(2000);
		}

		//Free resources and close sdl
		close();
	}

	return 0;
}

//low level C-styled error checking happens here.
//Initialize SDL components and create a window to display
bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create Window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL ERROR: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
			if (gScreenSurface == NULL)
			{
				printf("Window screen surface could not be created! SDL ERROR: %s\n", SDL_GetError());
				success = false;
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Load splash image
	std::string imagePath(SDL_GetBasePath());
	imagePath += "resources\\hello_world.bmp";
//	printf("%s\n", imagePath.c_str());
	
	gHelloWorld = SDL_LoadBMP(imagePath.c_str());
	if (gHelloWorld == NULL)
	{
		printf("Unable to load image %s! SDL ERROR: %s\n", imagePath.c_str(), SDL_GetError());
		return false;
	}

	return true;
}

void close()
{
	//Deallocate surface
	SDL_FreeSurface(gHelloWorld);
	gHelloWorld = NULL;//point to null address, if not done then gHelloWorld points to the memory address where the surface was at (memory contents of its address).

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}
