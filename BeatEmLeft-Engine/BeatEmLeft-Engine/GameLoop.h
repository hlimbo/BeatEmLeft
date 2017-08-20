#ifndef GAMELOOP_H
#define GAMELOOP_H
#include <SDL2/SDL_timer.h>
#include <string>

struct SDL_Window;

namespace GameLoop
{
	/*
		Should be treated only as a single instance
		do no modify the variables values in this struct
		they are meant to be read only
	*/
	struct GameLoopData
	{
		//measured in milliseconds
		float observedDeltaTime;
		//measured in seconds
		float deltaTime;
		Uint64 observedFPS;
		//measured in milliseconds since the start of the game loop
		float currentTime;
		float pastTime;
		//measured in counts per second obtained from high resolution counter
		Uint64 performanceFrequency;
		//measured in the number of counts (cycles) cpu has made
		Uint64 startCount;
		Uint64 endCount;
	};

	//call this before the start of the game loop
	void InitTimer();
	void UpdateFPS();
	//returns true if FPS was capped to target delta time, false otherwise
	bool CapFramerate(float targetDeltaTime);
	//returns the updated current time
	float UpdateCurrentTime();

	void DisplayFPS(SDL_Window* window,const float& updateDelayInMS);

	Uint64 FPS();
	//delta time in milliseconds
	float DeltaTimeMS();
	//delta time in seconds
	float DeltaTime();
}


#endif
