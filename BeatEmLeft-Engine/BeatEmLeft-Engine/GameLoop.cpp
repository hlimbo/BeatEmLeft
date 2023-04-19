#include "GameLoop.h"
#include <SDL_video.h>

static GameLoop::GameLoopData data;

void GameLoop::InitTimer()
{
	data.currentTime = 0.0f;
	data.performanceFrequency = SDL_GetPerformanceFrequency();
	data.startCount = SDL_GetPerformanceCounter();
}

void GameLoop::UpdateFPS()
{
	data.endCount = SDL_GetPerformanceCounter();
	data.observedDeltaTime = (1000.0f * (data.endCount - data.startCount)) / data.performanceFrequency;
	data.observedFPS = data.performanceFrequency / (data.endCount - data.startCount);
}
bool GameLoop::CapFramerate(float targetDeltaTime)
{
	bool canCapFramerate = targetDeltaTime - data.observedDeltaTime > 0.0f;
	if (canCapFramerate)
	{
		SDL_Delay((Uint32)(targetDeltaTime - data.observedDeltaTime));
		//Note: must re-record the times after the delay since the observed delta time retrieved was under target delta time
		data.endCount = SDL_GetPerformanceCounter();
		data.observedDeltaTime = (1000.0f * (data.endCount - data.startCount)) / data.performanceFrequency;
		data.observedFPS = data.performanceFrequency / (data.endCount - data.startCount);
	}

	return canCapFramerate;
}
float GameLoop::UpdateCurrentTime()
{
	data.currentTime += data.observedDeltaTime;
	data.deltaTime = data.observedDeltaTime / 1000.0f;
	data.startCount = data.endCount;

	return data.currentTime;
}

void GameLoop::DisplayFPS(SDL_Window* window,const float& updateDelayInMS)
{
	if (data.currentTime - data.pastTime >= updateDelayInMS)
	{
		data.pastTime = data.currentTime;
		std::string title("Beat Em Left | FPS: " + std::to_string(data.observedFPS));
		SDL_SetWindowTitle(window, title.c_str());
	}
}

Uint64 GameLoop::FPS()
{
	return data.observedFPS;
}

float GameLoop::DeltaTimeMS()
{
	return data.observedDeltaTime;
}

float GameLoop::DeltaTime()
{
	return data.deltaTime;
}
