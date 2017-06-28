#include "Core.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

int DrawPoints(SDL_Renderer* renderer,int x, int y, int p, int q)
{
	SDL_Point points[8];
	points[0] = SDL_Point{ x + p, y + q };
	points[1] = SDL_Point{ x - p, y + q };
	points[2] = SDL_Point{ x + p, y - q };
	points[3] = SDL_Point{ x - p, y - q };
	points[4] = SDL_Point{ x + q, y + p };
	points[5] = SDL_Point{ x - q, y + p };
	points[6] = SDL_Point{ x + q, y - p };
	points[7] = SDL_Point{ x - q, y - p };

	return SDL_RenderDrawPoints(renderer, points, 8);
}

//bonehead move ~ was subtracting the wrong values
//x and y are the circle's center
//p = x coordinate to draw pixel at
//q = y coordinate to draw pixel at
void CalcPointsForCircle(SDL_Point* points, int startIndex,int x,int y,int p,int q)
{
	points[startIndex] =	 SDL_Point{ x - p, y - q };
	points[startIndex + 1] = SDL_Point{ x - q, y - p };
	points[startIndex + 2] = SDL_Point{ x + q, y - p };
	points[startIndex + 3] = SDL_Point{ x + p, y - q };
	points[startIndex + 4] = SDL_Point{ x - p, y + q };
	points[startIndex + 5] = SDL_Point{ x - q, y + p };
	points[startIndex + 6] = SDL_Point{ x + q, y + p };
	points[startIndex + 7] = SDL_Point{ x + p, y + q };
}

void DrawCircle(SDL_Renderer* renderer, int cx, int cy,int radius)
{
	//draw the circle here using Bresenham’s Algorithm
	//1. set the center of the circle and radius
	SDL_Point center{ cx, cy };
	SDL_RenderDrawPoint(renderer, center.x, center.y);
	int p = 0;
	int q = radius;
	int numPixelsToDraw = q * 8;
	SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * numPixelsToDraw);
	//2. d = decision parameter which acts as a function
	//which helps decide to draw the next pixel to its horizontally adjacent neighbor or its diagonally adjacent neighbor
	int d = 3 - 2 * radius;
	//3. 
	while (p < q)
	{
		//DrawPoints(renderer, center.x, center.y, p, q);
		//slightly better optimization:: batch all points into one draw call ~ this is better than drawing each point per iteration by about 5 to 10 frames per second improvement
		//e.g. instead of fps being reduced to high 30s, fps is reduced to mid to high 40s
		CalcPointsForCircle(points, p * 8, center.x, center.y, p, q);
		++p;
		if (d < 0)
		{
			d = d + (4 * p) + 6;
		}
		else
		{
			d = d + (4 * (p - q)) + 10;
			--q;
		}
	}

	//printf("DrawCircle number of pixels drawn:: %d\n", numPixelsToDraw);
	SDL_RenderDrawPoints(renderer, points, numPixelsToDraw);
	free(points);
	//printf("Number of calls to DrawPoints: %d\n", p * 2);
}

int main(int argc, char* argv[])
{
	Core core;//initializes all necessary SDL Subsystems, and sets the window size

	SDL_SetRenderDrawColor(core.getRenderer(), 0, 0, 0, 0);
	SDL_RenderClear(core.getRenderer());
	SDL_RenderPresent(core.getRenderer());

	SDL_SetRenderDrawColor(core.getRenderer(), 0, 255, 0, 0);
	DrawCircle(core.getRenderer(), 250, 0, 100);
	SDL_RenderPresent(core.getRenderer());

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
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
		}

		//for (int i = 0;i < 50;++i)//wow frames aren't dropping hard!
		//{
		//	DrawCircle(core.getRenderer(), (i * 20) + 10, SCREEN_HEIGHT / 2, 10);
		//}

		SDL_RenderPresent(core.getRenderer());

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

		//if ((int)currentTime % 100 == 0)
		//{
		//	printf("currentTime: %f | observedFPS: %d | observedDeltaTime: %f\n", currentTime, observedFPS, observedDeltaTime);
		//}

		if ((int)currentTime % 100 == 0)
		{
			//display fps text in title
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}

	}

	return 0;
}