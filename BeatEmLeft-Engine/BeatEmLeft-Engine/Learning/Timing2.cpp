#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

/*
	What I need to focus on the game loop:
	1. Keep track of time passed
	2. Target FPS = 30 ( Target time per frame = 33.33 ms )

	Create a game loop that keeps track of the time passed since the last frame.
	Output: Display time on console in ms.

	Purpose of making a game loop: a game runs consistently across different computers regardless of how much one computer is better than another.
	Measure the consistency based on FPS!
*/


//target delta time = 33.33 seconds
//calculate observed delta time.

int targetFPS = 30;
float targetDeltaTime = 1000.0f / targetFPS;// in ms.


//    to get how many frames

float observedFPS;
float observedDeltaTime;

//my next task: retrieve my refresh rate of my computer using sdl function calls.
int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	//int startTime = SDL_GetTicks();
	//int endTime;
	//while (true)
	//{
	//	//sdl delay sleeps this process for targetDeltaTime at minimum ~ we are at the mercy of the OS Scheduler (it could take longer to return to this process).
	//	SDL_Delay(targetDeltaTime);
	//	endTime = SDL_GetTicks();

	//	observedDeltaTime = endTime - startTime;
	//	cout << "Target Delta Time: " << targetDeltaTime << endl;
	//	cout << "Observed Delta Time: " << observedDeltaTime << endl;

	//	startTime = endTime;
	//}

	//used in processors in hardware ~ getting rougly 2.3 mhz
	//getperformance frequency tells us how much work has been done in a cycle.
	//cout << "Get Performance Frequency: " << SDL_GetPerformanceFrequency() << endl;
	//cin.ignore();


	//getperformancecounter https://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx
	//this corresponds to wall clock time (can be used to correlate with REAL TIME)

	while (true)
	{
		Uint32 start32 = SDL_GetTicks();
		Uint64 start = SDL_GetPerformanceCounter();
		//SDL_Delay(targetDeltaTime);
		for (int i = 0;i < 100000; ++i) {}
		Uint32 end32 = SDL_GetTicks();
		Uint64 end = SDL_GetPerformanceCounter();

		Uint32 ticksElapsed = end32 - start32;
		Uint64 performanceElapsed = end - start;
		cout << "Ticks elapsed: " << ticksElapsed << endl;
		cout << "Performance elapsed: " << 1000.0f * performanceElapsed / SDL_GetPerformanceFrequency() << endl;

		// observedFPS =  x count/second * 1 frame / y counts = result frames / second;
		observedFPS = SDL_GetPerformanceFrequency() / performanceElapsed;
		cout << "Observed FPS: " << observedFPS << endl;
	}


	SDL_Quit();
	return 0;
}





//---------------------------------------------------------------------------------------



//sdl documentation example on GetPerformanceFrequency()
//#define DEFAULT_RESOLUTION  1
//
//static int ticks = 0;
//
//static Uint32 SDLCALL
//ticktock(Uint32 interval, void *param)
//{
//	++ticks;
//	return (interval);
//}
//
//static Uint32 SDLCALL
//callback(Uint32 interval, void *param)
//{
//	SDL_Log("Timer %d : param = %d", interval, (int)(uintptr_t)param);
//	return interval;
//}
//
//int
//main(int argc, char *argv[])
//{
//	int i, desired;
//	SDL_TimerID t1, t2, t3;
//	Uint32 start32, now32;
//	Uint64 start, now;
//
//	/* Enable standard application logging */
//	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
//
//	if (SDL_Init(SDL_INIT_TIMER) < 0) {
//		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
//		return (1);
//	}
//
//	/* Start the timer */
//	desired = 0;
//	if (argv[1]) {
//		desired = SDL_atoi(argv[1]);
//	}
//	if (desired == 0) {
//		desired = DEFAULT_RESOLUTION;
//	}
//	t1 = SDL_AddTimer(desired, ticktock, NULL);
//
//	/* Wait 10 seconds */
//	SDL_Log("Waiting 10 seconds");
//	SDL_Delay(10 * 1000);
//
//	/* Stop the timer */
//	SDL_RemoveTimer(t1);
//
//	/* Print the results */
//	if (ticks) {
//		SDL_Log("Timer resolution: desired = %d ms, actual = %f ms",
//			desired, (double)(10 * 1000) / ticks);
//	}
//
//	/* Test multiple timers */
//	SDL_Log("Testing multiple timers...");
//	t1 = SDL_AddTimer(100, callback, (void *)1);
//	if (!t1)
//		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create timer 1: %s", SDL_GetError());
//	t2 = SDL_AddTimer(50, callback, (void *)2);
//	if (!t2)
//		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create timer 2: %s", SDL_GetError());
//	t3 = SDL_AddTimer(233, callback, (void *)3);
//	if (!t3)
//		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create timer 3: %s", SDL_GetError());
//
//	/* Wait 10 seconds */
//	SDL_Log("Waiting 10 seconds");
//	SDL_Delay(10 * 1000);
//
//	SDL_Log("Removing timer 1 and waiting 5 more seconds");
//	SDL_RemoveTimer(t1);
//
//	SDL_Delay(5 * 1000);
//
//	SDL_RemoveTimer(t2);
//	SDL_RemoveTimer(t3);
//
//	start = SDL_GetPerformanceCounter();
//	for (i = 0; i < 1000000; ++i) {
//		ticktock(0, NULL);
//	}
//	now = SDL_GetPerformanceCounter();
//	SDL_Log("1 million iterations of ticktock took %f ms", (double)((now - start) * 1000) / SDL_GetPerformanceFrequency());
//
//	SDL_Log("Performance counter frequency: %\"SDL_PRIu64\"", (unsigned long long) SDL_GetPerformanceFrequency());
//	start32 = SDL_GetTicks();
//	start = SDL_GetPerformanceCounter();
//	SDL_Delay(1000);
//	now = SDL_GetPerformanceCounter();
//	now32 = SDL_GetTicks();
//	SDL_Log("Delay 1 second = %d ms in ticks, %f ms according to performance counter", (now32 - start32), (double)((now - start) * 1000) / SDL_GetPerformanceFrequency());
//
//	SDL_Quit();
//	return (0);
//}

//--------------------------------------

//my checks
//int main(int argc, char* argv[])
//{
//
//	int x1 = 50000000;
//	Uint64 startCount = SDL_GetPerformanceCounter();
//	for (int i = 0;i < x1;++i) {}
//	cout << "hi" << endl;
//	Uint64 endCount = SDL_GetPerformanceCounter();
//
//	//gives you the number of miliseconds passed since cout call ~ this method gives you a MORE PRECISE calculation of the time it took for code logic to run.
//	float milisecondsOfCallingCout = ((float)(endCount - startCount) / SDL_GetPerformanceFrequency()) * 1000.0f;
//	cout << milisecondsOfCallingCout << endl;
//	//cin.ignore();
//
//
//	//this gives you a less precise calculation of the time it took for code logic to run
//	int x2 = 50000000;
//	Uint32 start = SDL_GetTicks();
//	for (int i = 0;i < x2;++i) {}
//	cout << "hi" << endl;
//	Uint32 now = SDL_GetTicks();
//
//	Uint32 milisecondsOfCallingCout2 = now - start;
//	cout << milisecondsOfCallingCout2 << endl;
//
//	float relativeError = ((milisecondsOfCallingCout2 - milisecondsOfCallingCout) / milisecondsOfCallingCout2) * 100.0f;
//	cout << "relative error % in time differences: " << relativeError << endl;
//
//	cin.ignore();
//
//	//while (true)
//	//{
//	//	Uint64 performanceCounter = SDL_GetPerformanceCounter();
//	//	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
//
//	//	cout << "Performance Counter: "  << performanceCounter << endl;
//	//	cout << "Performance Frequency: " << performanceFrequency << endl;
//
//	//	cout << "Performance Counter / Performance Frequency = " << (performanceCounter / performanceFrequency) << endl;
//
//	//	SDL_Delay(500);
//	//}
//
//
//
//	return 0;
//}