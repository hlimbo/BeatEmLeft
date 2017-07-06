#include <stdio.h>
#include <string>
#include <SDL2/SDL.h>

#include "../Core.h"
#include "../Sprite.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	Sprite background;
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
	cameraRect.y = 0;

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

		//update
		background.MoveSprite(0, 1);
		//cameraRect.y -= 1;

		//rendering
		background.DrawSprite(render,&cameraRect);
		player.DrawSprite(render);


		SDL_RenderPresent(render);
		SDL_RenderClear(render);

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


	return 0;
}