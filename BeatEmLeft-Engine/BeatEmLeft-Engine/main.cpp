#include "Core.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <SDL2/SDL_image.h>
//#include <vld.h>


//long term goal: condense low level sdl function calls into user friendly high level function calls

//sprite can be defined as a subset of a texture or a sprite can be treated as one texture.
//spritesheets are treated as one texture ~ can render different parts of the texture (retrieve each sprite piece to render).

//display observed fps on screen.
int main(int argc, char* argv[])
{
	Uint32 observedFPS;
	float observedDeltaTime;
	Core core;

	TTF_Font* font;
	std::string mainPath(SDL_GetBasePath());
	mainPath += std::string("resources\\");

	std::string fontPath(mainPath);
	fontPath += std::string("RubikMonoOne-Regular.ttf");
	font = TTF_OpenFont(fontPath.c_str(), 24);
	if (!font)
	{
		printf("Error: %s\n", TTF_GetError());
		return 2;
	}


	int width, height;
	SDL_RenderSetLogicalSize(core.getMainRenderer(), 640, 480);
	SDL_RenderGetLogicalSize(core.getMainRenderer(), &width, &height);
	char t1[10];
	char t2[10];

	SDL_itoa(width, t1, 10);
	SDL_itoa(height, t2, 10);

	float scaleX, scaleY;
	SDL_RenderGetScale(core.getMainRenderer(), &scaleX, &scaleY);
	std::string s1 = std::to_string(scaleX);
	std::string s2 = std::to_string(scaleY);

	SDL_Color Black = {0,0,0};
	SDL_Surface* text_surface = TTF_RenderText_Solid(font, s1.c_str(), Black);
	if (!text_surface)
	{
		printf("%s\n", TTF_GetError());
		return 2;
	}

	SDL_Texture* text = SDL_CreateTextureFromSurface(core.getMainRenderer(), text_surface);
	SDL_FreeSurface(text_surface);
	if (!text)
	{
		printf("%s\n", TTF_GetError());
		return 2;
	}

	SDL_Rect Message_rect;
	Message_rect.x = 0;
	Message_rect.y = 0;
	Message_rect.w = 100;
	Message_rect.h = 100;

	SDL_Color White = { 255,255,255 };
	std::string bgPath(mainPath);
	bgPath += std::string("hello_world.bmp");
	SDL_Surface* surf = SDL_LoadBMP(bgPath.c_str());
	SDL_Texture* bgText = SDL_CreateTextureFromSurface(core.getMainRenderer(), surf);
	SDL_FreeSurface(surf);
	
	SDL_Rect dstRect;
	dstRect.h = 150;
	dstRect.w = 150;
	dstRect.x = 50;
	dstRect.y = 50;

	/*IMG_LoadTexture(core.getMainRenderer(), bgPath.c_str());*/
	//SDL_Texture* bad = IMG_LoadTexture(core.getMainRenderer(), "badPath!");
	//if (bad == NULL)
	//{
	//	printf("Bad path: %s\n", IMG_GetError());
	//	return 5;
	//}

	//~ can use this to hide objects from screen hehe
	//SDL_RenderGetClipRect(core.getMainRenderer(), &dstRect);

	SDL_SetRenderDrawColor(core.getMainRenderer(), 255, 0, 255, 255);
	SDL_RenderClear(core.getMainRenderer());
	SDL_RenderSetScale(core.getMainRenderer(), 1.0f, 1.0f);
	SDL_RenderCopy(core.getMainRenderer(), bgText, NULL, &dstRect);
	SDL_RenderSetScale(core.getMainRenderer(), 1.0f, 1.0f);
	SDL_RenderCopy(core.getMainRenderer(), text, NULL, &Message_rect);
	SDL_RenderPresent(core.getMainRenderer());


	//definitely need a texture store class.
#define SIZE 2
	SDL_Texture* textures[SIZE];

	//-------------------------- Game Loop ------------------------------------
	//measured in ms ~ does not count starting from SDL system initialization.
	float currentTime = 0.0f;

	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
	bool running = true;
	while (running)
	{
		//TODO: process inputs
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
		}

		//TODO: process game logic updates

		//TODO: render game

		//fps seems to drop by 10 frames when having these lines here.
		//observation ~ loading bmps to a surface, converting them into a texture, and freeing their surfaces costs ~10 frames.
	/*	SDL_Color White = { 255,255,255 };
		std::string bgPath(mainPath);
		bgPath += std::string("hello_world.bmp");
		SDL_Surface* surf = SDL_LoadBMP(bgPath.c_str());
		SDL_Texture* bgText = SDL_CreateTextureFromSurface(core.getMainRenderer(), surf);
		SDL_FreeSurface(surf);
		SDL_DestroyTexture(bgText);*/

		//SDL_RenderClear(core.getMainRenderer());

		//load in the same texture 500 times
		//SDL_Surface* loadSurf = NULL;
		//std::string imgPath(mainPath);
		//imgPath += std::string("block.png");
		//for (int i = 0;i < SIZE; ++i)
		//{	
		//	loadSurf = IMG_Load(imgPath.c_str());
		//	textures[i] = SDL_CreateTextureFromSurface(core.getMainRenderer(), loadSurf);
		//	SDL_FreeSurface(loadSurf);
		//}

		////Draw images onto renderer back buffer
		////32 wide x 24 tall, x = 0px, y = 0px
		//SDL_Rect dstRect;
		//dstRect.w = 32;
		//dstRect.h = 24;
		//dstRect.x = 0;
		//dstRect.y = 0;
		//for (int i = 0; i < SIZE; ++i)
		//{
		//	dstRect.x = i * 32;
		//	dstRect.y = i * 24;
		//	SDL_RenderCopy(core.getMainRenderer(), textures[i], NULL, &dstRect);
		//}

		////flip images to front buffer
		//SDL_RenderPresent(core.getMainRenderer());

		////Destroy all 500 textures
		//for (int i = 0; i < SIZE; ++i)
		//{
		//	SDL_DestroyTexture(textures[i]);
		//}
		
		endCount = SDL_GetPerformanceCounter();
		Uint32 countElapsed = (Uint32)(endCount - startCount);
		observedDeltaTime = 1000.0f * countElapsed / performanceFrequency;
		observedFPS = (Uint32)performanceFrequency / countElapsed;
		currentTime += observedDeltaTime;
		
		//update only every 100 ms
		if ((int)currentTime % 100 == 0)
		{
			//display fps text in title
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getMainWindow(), title.c_str());
		}

		float msDifference = core.getTargetDeltaTime() - observedDeltaTime;

		//if device finishes a cycle faster than targetDeltaTime, sleep for a bit.
		if (msDifference > 0)
		{
			Uint32 msToSleep = (Uint32)(observedDeltaTime + msDifference);
			SDL_Delay(msToSleep);
		}
		else //if msDifference <= 0 ~ if device is taking longer to process due to OS scheduling, try to sleep enough to hit targetFPS
		{
			SDL_Delay((Uint32)core.getTargetDeltaTime());
		}

		startCount = endCount;
	}

	// ---------------------------------------- End Game Loop ----------------------------------------/

	//if (TTF_WasInit())
	//	TTF_Quit();
	//
	//IMG_Quit();

	return 0;
}
