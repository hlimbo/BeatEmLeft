#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>

#include "../Core.h"
#include "../Sprite.h"
#include "../Utility/TextureLoader.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	
	//grabbing the write filepaths...
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");
	string playerPath = mainPath + string("block.jpg");
	string backgroundPath = mainPath + string("Background.png");


	//using TextureLoader utility
	SDL_Texture* background = TextureLoader::Load(render, backgroundPath.c_str());
	SDL_Texture* player = TextureLoader::Load(render, playerPath.c_str());
	
	SDL_Rect bgBounds, playerBounds;
	int bgAccess, playerAccess;

	//textures are static, they change rarely.. by default
	//makes sense since I won't be modifying the pixel arrangement of the texture.
	SDL_QueryTexture(background, NULL, &bgAccess, &bgBounds.w, &bgBounds.h);
	SDL_QueryTexture(player, NULL, &playerAccess, &playerBounds.w, &playerBounds.h);

	printf("bgAccess: %d\n", bgAccess);
	printf("playerAccess: %d\n", playerAccess);

	bgBounds.x = 0;
	bgBounds.y = 0;
	bgBounds.w = bgBounds.w - bgBounds.h * 0.0;
	bgBounds.h = bgBounds.h - bgBounds.h * 0.0;

	playerBounds.x = SCREEN_WIDTH / 2;
	playerBounds.y = SCREEN_HEIGHT / 2;
	//this is how image scale can change...scale down by percentage
	//keep a copy of the original dimensions of the texture after it is
	//loaded and queried
	playerBounds.w = playerBounds.w - playerBounds.w * 0.85;
	playerBounds.h = playerBounds.h - playerBounds.h * 0.85;

	//the third parameter(null) is used for camera scrolling and such...
	SDL_RenderCopy(render, background, NULL, &bgBounds);
	SDL_RenderCopy(render, player, NULL, &playerBounds);

	printf("bgBounds: (%d, %d), (%d,%d)\n", bgBounds.x, bgBounds.y, bgBounds.w, bgBounds.h);
	printf("playerBounds: (%d, %d), (%d,%d)\n", playerBounds.x, playerBounds.y, playerBounds.w, playerBounds.h);


	SDL_Rect cameraRect;
	//width and height act as zoom controls
	cameraRect.w = SCREEN_WIDTH;
	cameraRect.h = SCREEN_HEIGHT;
	cameraRect.x = 0;
	cameraRect.y = 0;

	//using sprite.h
	/*Sprite background;
	string backgroundPath = mainPath + string("Background.png");
	background.LoadSprite(render, backgroundPath.c_str());
	background.SetLocation(0, 0);

	Sprite player;
	string playerPath = mainPath + string("block.jpg");
	player.LoadSprite(render, playerPath.c_str());
	player.SetLocation(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	player.SetSize(50, 50);

	SDL_Rect cameraRect;
	cameraRect.h = SCREEN_HEIGHT * 2;
	cameraRect.w = SCREEN_WIDTH * 2;
	cameraRect.x = 0;
	cameraRect.y = 0;*/

	//---------------- Game Loop ------------------//

	float observedDeltaTime = core.getTargetDeltaTime();
	Uint32 observedFPS = core.getTargetFPS();
	float currentTime = 0.0f;
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
	bool running = true;
	while (running)
	{
		//events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
		}

		//using TextureLoader utility textures
		SDL_RenderCopy(render, background,&cameraRect, &bgBounds);
		SDL_RenderCopy(render, player, NULL, &playerBounds);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);

		//Using Sprite.h
		////update
		//background.MoveSprite(0, 1);
		////cameraRect.y -= 1;

		////rendering
		//background.DrawSprite(render,&cameraRect);
		//player.DrawSprite(render);


		//SDL_RenderPresent(render);
		//SDL_RenderClear(render);

		//cap framerate here
		endCount = SDL_GetPerformanceCounter();
		Uint32 countElapsed = (Uint32)(endCount - startCount);
		observedDeltaTime = 1000.0f * countElapsed / SDL_GetPerformanceFrequency();
		observedFPS = (Uint32)SDL_GetPerformanceFrequency() / countElapsed;
		currentTime += observedDeltaTime;

		float msDifference = core.getTargetDeltaTime() - observedDeltaTime;

		if (msDifference != 0)
		{
			if (msDifference > 0)
			{
				Uint32 msToSleep = (Uint32)(observedDeltaTime + msDifference);
				SDL_Delay(msToSleep);
			}
			else
			{
				SDL_Delay((Uint32)core.getTargetDeltaTime());
			}
		}

		startCount = endCount;

		//display fps text in title
		if ((int)currentTime % 100 == 0)
		{
			
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}

	}

	//Free the textures
	TextureLoader::Free(background);
	TextureLoader::Free(player);


	return 0;
}