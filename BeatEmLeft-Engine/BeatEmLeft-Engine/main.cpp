#include "Core.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <SDL2/SDL_image.h>
//#include <vld.h>
#include "Sprite.h"


//long term goal: condense low level sdl function calls into user friendly high level function calls

//sprite can be defined as a subset of a texture or a sprite can be treated as one texture.
//spritesheets are treated as one texture ~ can render different parts of the texture (retrieve each sprite piece to render).

//display observed fps on screen.
int main(int argc, char* argv[])
{
	Uint32 observedFPS;
	float observedDeltaTime;
	Core core;

	//Question: Should I put this in its own static helper class?
	std::string mainPath(SDL_GetBasePath());
	mainPath += std::string("resources\\");

	//Load Sprite from disk
	std::string spritePath(mainPath);
	spritePath += std::string("block.jpg");
	Sprite firstSprite;
	firstSprite.LoadSprite(core.getRenderer(), spritePath.c_str());
	firstSprite.SetSize(40, 40);

	SDL_Point newLocation;
	newLocation.x = SCREEN_WIDTH / 2 - firstSprite.GetWidth() / 2;
	newLocation.y = SCREEN_HEIGHT / 2 - firstSprite.GetHeight() / 2;

	firstSprite.MoveSprite(newLocation);
	firstSprite.DrawSprite(core.getRenderer());
	int move = 10;

	printf("Sprite dimensions: %d, %d", firstSprite.GetWidth(), firstSprite.GetHeight());

	//definitely need a texture store class.
//#define SIZE 2
//	SDL_Texture* textures[SIZE];

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
		if(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
			//temporary code.
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					firstSprite.MoveSprite(0, -move);
					break;
				case SDLK_s:
					firstSprite.MoveSprite(0, move);
					break;
				case SDLK_a:
					firstSprite.MoveSprite(-move, 0);
					break;
				case SDLK_d:
					firstSprite.MoveSprite(move, 0);
					break;
				default:
					firstSprite.SetLocation(newLocation);
					break;
				}
			}
		}

		//TODO: process game logic updates
		//core.update();

		//TODO: render game
		//core.render();
		SDL_RenderClear(core.getRenderer());
		firstSprite.DrawSprite(core.getRenderer());
		
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
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
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

	firstSprite.FreeSprite();

	return 0;
}

//PUT UNDER TODO RENDER!

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
