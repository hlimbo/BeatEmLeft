#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

#include "../Core.h"
#define WIDTH 480
#define HEIGHT 480


//helper method used to plot points to the window to construct a circle.
void CalcCirclePoints(SDL_Point* points, int startIndex, int cx, int cy, int x, int y);
int DrawCircle(SDL_Renderer* renderer, int cx, int cy, int radius);
int DrawCircle(SDL_Renderer* renderer, SDL_Point center, int radius);

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	SDL_Point center{ WIDTH / 2, HEIGHT / 2 };
	SDL_Point stickLocation{ center.x, center.y };
	SDL_Point rawLocation{ center.x, center.y };

	//setup and initialize game controller - can open either an XInput (xbox controller) OR PS4 controller
	SDL_GameController* controller = SDL_GameControllerOpen(0);
	if(controller == NULL)
		fprintf(stderr,"Error: Controller: %s\n", SDL_GetError());

	const char* name = SDL_GameControllerName(controller);
	printf("Controller Name: %s\n", name);

	const char* mapping = SDL_GameControllerMapping(controller);
	printf("%s\n", mapping);

	if (SDL_GameControllerGetAttached(controller))
	{
		const char* controllerType = SDL_GameControllerName(controller);
		printf("Controller: %s is attached\n", controllerType);
	}

	//pre-render static images (e.g. images that do not move)
		//this saves TONS of FPS waste since we don't have to recalculate where to draw the pixels e.g. circles every frame

	//1st step is to save whatever will be rendered on this texture to the ram.
	SDL_Texture* background = SDL_CreateTexture(render, SDL_GetWindowPixelFormat(core.getWindow()), SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
	//2nd step ensures that whatever that will be rendered does not go to the screen but instead gets rendered to the texture background
	SDL_SetRenderTarget(render, background);
	//rendering to background texture only (NOT to screen)
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);
	
	SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
	//center point
	SDL_RenderDrawPoint(render, center.x, center.y);
	//outer circle
	DrawCircle(render, center, 150);
	//inner circle
	DrawCircle(render, center, 8);

	SDL_SetRenderDrawColor(render, 100, 100, 100, 100);
	//vertical line
	SDL_RenderDrawLine(render, WIDTH / 2, 0, WIDTH / 2, HEIGHT);
	//horizontal line
	SDL_RenderDrawLine(render, 0, HEIGHT / 2, WIDTH, HEIGHT / 2);

	//middle circle
	SDL_SetRenderDrawColor(render, 0, 150, 145, 255);
	DrawCircle(render, center, 140);
	SDL_RenderPresent(render);//render presents its drawings to the background texture only

	//set the render target back to the main screen
	SDL_SetRenderTarget(render, NULL);


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
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
			else
			{
#define MAX_VALUE 32768
#define DEADZONE 1639

				if (event.type == SDL_CONTROLLERBUTTONDOWN)
				{
					if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
					{
						puts("A");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
					{
						puts("B");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X))
					{
						puts("X");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y))
					{
						puts("Y");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
					{
						puts("LeftShoulder");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
					{
						puts("RightShoulder");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_GUIDE))
					{
						puts("Guide");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START))
					{
						puts("Start");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK))
					{
						puts("Back");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSTICK))
					{
						puts("LeftStick");
					}
					else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK))
					{
						puts("RightStick");
					}
					
				}

				if (event.type == SDL_CONTROLLERAXISMOTION)
				{
					int scale = 150;
					int px = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
					int py = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

					rawLocation.x = (int)(((float)px / MAX_VALUE) * scale) + center.x;
					rawLocation.y = (int)(((float)py / MAX_VALUE) * scale) + center.y;
					
					float ux;
					float uy;
					
					//INNER DEADZONES
					//adjust for deadzones 1639 / 32768 is about 5%  inner deadzone
					if (px < -DEADZONE || px > DEADZONE) //register as analog stick movement along x-axis
						ux = (float)px / MAX_VALUE;
					else
						ux = 0.0f;

					if (py < -DEADZONE || py > DEADZONE) //register as analog stick movement along y-axis
						uy = (float)py / MAX_VALUE;
					else
						uy = 0.0f;
			
					stickLocation.x = (int)(ux * scale) + center.x;
					stickLocation.y = (int)(uy * scale) + center.y;

					//printf("StickLocation: (%d, %d)\n", stickLocation.x, stickLocation.y);

				}
			}
		}

		//take a copy of the background pre-rendered (e.g. all the pixels required to draw it are saved in ram already as a texture..)
		//so just render it to the main screen.
		SDL_RenderCopy(render, background, NULL, NULL);

		//processed joystick visual
		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		DrawCircle(render, stickLocation, 5);
		//raw joystick visual
		SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
		DrawCircle(render, rawLocation, 3);

		SDL_RenderPresent(render);


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

	//CLEANUP
	if (controller != NULL)
		SDL_GameControllerClose(controller);

	return 0;
}

void CalcCirclePoints(SDL_Point* points, int startIndex, int cx, int cy, int x, int y)
{
	points[startIndex] =	SDL_Point{ cx - x, cy - y };
	points[startIndex + 1] = SDL_Point{ cx - y, cy - x };
	points[startIndex + 2] = SDL_Point{ cx + y, cy - x };
	points[startIndex + 3] = SDL_Point{ cx + x, cy - y };
	points[startIndex + 4] = SDL_Point{ cx - x, cy + y };
	points[startIndex + 5] = SDL_Point{ cx - y, cy + x };
	points[startIndex + 6] = SDL_Point{ cx + y, cy + x };
	points[startIndex + 7] = SDL_Point{ cx + x, cy + y };
}

int DrawCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
	//draw the circle here using Bresenham’s Algorithm
	int p = 0;
	int q = radius;
	int numPixelsToDraw = q * 8;
	SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * numPixelsToDraw);
	//d = decision parameter which acts as a function
	//which helps decide to draw the next pixel to its horizontally adjacent neighbor or its diagonally adjacent neighbor
	int d = 3 - 2 * radius;

	while (p < q)
	{
		//slightly better optimization:: batch all points into one draw call ~ draw calls are expensive
		CalcCirclePoints(points, p * 8, cx, cy, p, q);
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

	int result = SDL_RenderDrawPoints(renderer, points, numPixelsToDraw);
	free(points);
	return result;
}

int DrawCircle(SDL_Renderer* renderer, SDL_Point center, int radius)
{
	return DrawCircle(renderer, center.x, center.y, radius);
}
