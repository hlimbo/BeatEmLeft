#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "../MasterHeader.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	ImageStore store(render);

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	//cout << "Enter in tiles map image file to load: ";
	string tilesName("tiles-stones.png");
	string tilesPath;
	//getline(cin, tilesName);
	//fflush(stdin);
	tilesPath = mainPath + tilesName;
	Image* srcImg = store.Load(tilesName, tilesPath);

	//these are widths and heights of the image relative to the source's texture map
	int src_tile_w = 64;
	int src_tile_h = 64;
	//cout << "Enter in src_tile_w: ";
	//cin >> tileWidth;
	//cout << "Enter in src_tile_h: ";
	//cin >> tileHeight;

	SpriteSheet tileSheet(render, srcImg, src_tile_w, src_tile_h);
	cout << "frameCount: " << tileSheet.GetFrameCount() << endl;

	for (int i = 0;i < tileSheet.GetFrameCount();++i)
	{
		const SDL_Rect* frame = tileSheet.GetFrame(i);
		cout << "frame " << i << ": " << "(" << frame->x << ", " << frame->y << ")" << endl;
	}

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
		}

		//these are widths and heights of each tile relative to the game's window
		int tileWidth = 24, tileHeight = 24;
		//render each tile from the tileSheet along the bottom side of the screen
		for (int i = 0;i < tileSheet.GetFrameCount();++i)
		{

			const SDL_Rect* frame = tileSheet.GetFrame(i);
			SDL_Rect dstRect;
			dstRect.x = i * 24;//frame->w;
			dstRect.y = SCREEN_HEIGHT - 24;
			dstRect.w = 24;//frame->w;
			dstRect.h = 24;//frame->h;

			SDL_RenderCopy(render, tileSheet.texture, frame, &dstRect);
		}

		//when a tile is selected,render the tile towards the center of the screen


		//SDL_RenderCopy(render, srcImg->texture, NULL, NULL);
		SDL_RenderPresent(render);
		SDL_RenderClear(render);

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

		//display fps text in title
		std::string title("Beat Em Left");
		title += std::string(" | FPS: ") + std::to_string(observedFPS);
		SDL_SetWindowTitle(core.getWindow(), title.c_str());
	}



	return 0;
}
