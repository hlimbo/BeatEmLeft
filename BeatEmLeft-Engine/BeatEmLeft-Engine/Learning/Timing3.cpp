#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
using namespace std;

#define WIDTH 640
#define HEIGHT 480

/*
	This program records how many milliseconds it takes to render 1 frame of an empty window.
	All recordings can be seen in stats.txt
*/

//problem - on every other tick, the loop runs as fast as it can.
//running the game loop as fast as it can takes up alot of CPU!
//taking up less CPU means less battery drainage = less electricity usage.
int main(int argc, char* argv[])
{
	ofstream myfile;
	myfile.open("stats.txt");

	SDL_Init(SDL_INIT_VIDEO);

	SDL_DisplayMode displayMode;

	SDL_Window* window = SDL_CreateWindow("Monitor FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//the monitor refresh rate will be our target FPS
	int targetFPS = -1;
	float targetDeltaTime = 0;//in ms
	int observedFPS = 0;
	float observedDeltaTime = 0;//in ms

	//should be part of the initialization
	int displayCount = SDL_GetNumVideoDisplays();
	if (displayCount > 0)
	{
		if (SDL_GetDisplayMode(0, 0, &displayMode) == 0)
		{
			targetFPS = displayMode.refresh_rate;
			targetDeltaTime = 1000.0f / targetFPS;
			cout << "targetFPS: " << targetFPS << endl;
			cout << "targetDeltaTime: " << targetDeltaTime << endl;
			cout << "monitor height: " << displayMode.h << endl;
			cout << "monitor width: " << displayMode.w << endl;
			cout << displayMode.format << endl;
		}
	}

	//measured in ms;
	float currentTime = 0.0f;

	//measured in counts per second
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
	bool quit = false;
	
	while (!quit)
	{
		//inputs should be encapsulated in its own thing
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		endCount = SDL_GetPerformanceCounter();
		Uint32 countElapsed = endCount - startCount;
		observedDeltaTime = 1000.0f * countElapsed / performanceFrequency;
		observedFPS = performanceFrequency / countElapsed;

		if (myfile.good())
		{
			myfile << "observedDeltaTime: " << observedDeltaTime << "\n";
			myfile << "observedFPS: " << observedFPS << "\n";
			myfile << "currentTime: " << currentTime << "\n";
			myfile << "currentTicks: " << SDL_GetTicks() << "\n";
		}

		float msDifference = targetDeltaTime - observedDeltaTime;

		//if computer finishes a cycle faster than targetDeltaTime, have the process sleep until it meets its target delta time.
		if (msDifference > 0)
		{
			myfile << "msDifference > 0" << "\n";
			Uint32 msToSleep = observedDeltaTime + msDifference;
			SDL_Delay(msToSleep);
		}
		else //if msDifference <= 0 ~ try to cap observedDeltaTime to targetDeltaTime
		{
			myfile << "msDifference " << msDifference << "\n";
			SDL_Delay(targetDeltaTime);
		}

		currentTime += observedDeltaTime;
		startCount = endCount;
	}

	myfile.close();
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}