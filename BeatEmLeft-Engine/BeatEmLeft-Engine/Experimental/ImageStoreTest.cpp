#include <stdio.h>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../Core.h"
#include "../Components/SpriteSheet.h"
#include "../Utility/TextureLoader.h"
#include "../Utility/ImageStore.h"
#include "../Utility/TextureStore.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");
	string objectsPath = mainPath + string("objects.png");
	string redBlockPath = mainPath + string("redblock.png");
	string whitePath = mainPath + string("white.png");

	ImageStore imageStore(render);
	imageStore.Load("objects.png", objectsPath);

	SDL_Rect frame0;
	frame0.x = frame0.y = 0;
	frame0.w = 50;
	frame0.h = 66;
	SDL_Rect frame1;
	frame1.x = 50;
	frame1.y = 0;
	frame1.w = 50;
	frame1.h = 66;
	SDL_Rect frame2;
	frame2.x = 100;
	frame2.y = 0;
	frame2.w = 50;
	frame2.h = 66;
	Uint8 newAlpha = 255;

	SDL_Rect* frame = &frame0;

	TextureStore textureStore(render);
	SDL_Texture* whiteTexture = textureStore.Load("white.png", whitePath);
	//textureStore.SetAlpha("white.png", 255 / 2);
	SDL_Rect whiteRect;
	whiteRect.x = whiteRect.y = 0;
	float wScale = 0.3333;
	whiteRect.w = SCREEN_WIDTH * wScale;
	whiteRect.h = SCREEN_HEIGHT;

	SDL_Rect dstRect;
	dstRect.x = 0;
	dstRect.y = 0;
	dstRect.w = SCREEN_WIDTH * wScale;
	dstRect.h = SCREEN_HEIGHT;

	//---------------- Game Loop ------------------//

	//observedDeltaTime is measured in milliseconds
	float observedDeltaTime = core.getTargetDeltaTime();
	float deltaTime = observedDeltaTime / 1000.0f;//converts from milliseconds to seconds
	//to avoid unnecessary context switches os might do (which I have no control over.. cache the target delta time)
	float targetDeltaTime = core.getTargetDeltaTime();
	Uint64 observedFPS = core.getTargetFPS();
	float currentTime = 0.0f;
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
	bool running = true;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					printf("left mouse click (%d,%d)\n", event.button.x, event.button.y);
				}
			}

			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_UP)
				{
					newAlpha += 2;
				}
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					newAlpha -= 2;
				}

				if (event.key.keysym.sym == SDLK_LEFT)
				{
					whiteRect.x -= SCREEN_WIDTH * wScale;
					if (whiteRect.x < 0)
						whiteRect.x = SCREEN_WIDTH - whiteRect.w;

					if (frame == &frame0)
						frame = &frame2;
					else if (frame == &frame1)
						frame = &frame0;
					else if (frame == &frame2)
						frame = &frame1;
				}
				else if (event.key.keysym.sym == SDLK_RIGHT)
				{
					whiteRect.x += SCREEN_WIDTH * wScale;
					if (whiteRect.x + 5 > SCREEN_WIDTH)
						whiteRect.x = 0;

					if (frame == &frame0)
						frame = &frame1;
					else if (frame == &frame1)
						frame = &frame2;
					else if (frame == &frame2)
						frame = &frame0;
				}

			}
		}

		imageStore.SetAlpha("objects.png", newAlpha, *frame);

		SDL_RenderClear(render);
		SDL_Texture* texture = imageStore.Get("objects.png")->texture;
		SDL_RenderCopy(render, whiteTexture, NULL, &whiteRect);
	
	/*	dstRect.x += 1;
		if (dstRect.x > SCREEN_WIDTH)
		{
			dstRect.x = 0;
		}*/
		SDL_RenderCopy(render, texture, NULL, NULL);
		SDL_RenderPresent(render);

		endCount = SDL_GetPerformanceCounter();
		observedDeltaTime = (1000.0f * (endCount - startCount)) / performanceFrequency;//gives ms
		observedFPS = performanceFrequency / (endCount - startCount);

		//if the computer can process the update and draw functions faster than 60 fps...
		//cap the frame-rate here to ensure that all computers play roughly around the same fps
		float msDifference = targetDeltaTime - observedDeltaTime;
		if (msDifference > 0)
		{
			SDL_Delay((Uint32)msDifference);
			//Note: must re-record the times after the delay since the times before the delay maybe
			//under 16.666 ms
			endCount = SDL_GetPerformanceCounter();
			observedDeltaTime = (1000.0f * (endCount - startCount)) / performanceFrequency;//gives ms
			observedFPS = performanceFrequency / (endCount - startCount);
		}

		currentTime += observedDeltaTime;
		deltaTime = observedDeltaTime / 1000.0f;
		startCount = endCount;

		std::string title("Beat Em Left");
		title += std::string(" | FPS: ") + std::to_string(observedFPS);
		SDL_SetWindowTitle(core.getWindow(), title.c_str());
	}

	return 0;
}