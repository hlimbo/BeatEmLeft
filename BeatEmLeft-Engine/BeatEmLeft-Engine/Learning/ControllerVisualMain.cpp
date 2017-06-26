#include "Core.h"
#include "GameConstants.h"
using namespace Contants;

#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <Vect2.h>// this is my own static library lol

/*
	Description: This main class will illustrate 
	raw analog stick input and button presses on a PS4 controller.

	Purpose: This class will remind me what analog stick processing looks like at a very low level.

	white dot = the processed input after deadzones have been taken into account.
	green dot = raw joystick input on the left analog stick.

	red circle = unit circle.
	blue inner circle = inner deadzone.
	blue outer circle = outer deadzone.

*/

//small wrapper struct to SDL_GameController that handles raw analog stick input and accepts input that is out of the inner deadzones.
struct AnalogData
{
	SDL_GameController* controller;
	int xInput;
	int yInput;

	int rawX;
	int rawY;

	//if returnCode == 1, that means the controllermotionaxis event got pushed into the event queue.
	//if returnCode == 0, that means the controllermotionaxis event will be dropped from the event queue.
	int returnCode;
};

void DisplayFPS(Core* core, Uint32 observedFPS, const float currentTime, int frequency);
void RenderCircle(Core* core, const SDL_Point& center, float radius, const SDL_Color& color, int sides);
void RenderLine(Core* core, const SDL_Point start, const SDL_Point end, const SDL_Color color);

void DrawAnalogDisplay(Core* core);
void DrawProcessedInput(Core* core, const SDL_Point newPos);
void DrawRawInput(Core* core, const SDL_Point newPos);

//left analog stick filter
int AnalogStickFilter(void* controllerdata, SDL_Event* event);

int main(int argc, char* argv[])
{

	//Setup renderer and other sdl subsystems.
	Core core;

	//set resources folder path
	std::string mainPath(SDL_GetBasePath());
	mainPath += std::string("resources\\");

	//setup and initialize game controller.
	SDL_GameController* controller = SDL_GameControllerOpen(0);
	if (controller == NULL) 
		printf("Error: Controller: %s\n", SDL_GetError());

	AnalogData analogInput;
	if (SDL_GameControllerGetAttached(controller))
	{
		const char* controllerType = SDL_GameControllerName(controller);
		printf("Controller: %s is attached\n", controllerType);

		//setup event filter for left analog stick input.
		analogInput.controller = controller;
		SDL_SetEventFilter(AnalogStickFilter, &analogInput);
	}


	//used for drawrawinput and drawprocessedinput;
	SDL_Point center{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

	SDL_Point rawInputPos = center;
	SDL_Point processedInputPos = center;

	//Setup game loop variables
	Uint32 observedFPS;
	float observedDeltaTime = 0.0f;
	float currentTime = 0.0f; //the time since game loop started running in ms.
	bool running = true;

	//counts per second : see https://msdn.microsoft.com/en-us/library/windows/desktop/ms644905(v=vs.85).aspx
	//counts = number of cycles counted by the cpu... does not measure time but is A GREAT WAY to gain nanosecond precision time!
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency(); //note: I have not tested to see if the frequency changes per update
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;

	//GAME LOOP
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//need to find a way to force quit via interrupt
			if (event.type == SDL_QUIT)
				running = false;
			else
			{
				//TODO: grab game controller analog stick events here
				if (event.type == SDL_CONTROLLERBUTTONDOWN)
				{
					if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A))
						puts("X");
					else if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B))
						puts("O");
					else if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y))
						puts("Triangle");
					else if (SDL_GameControllerGetButton(controller, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X))
						puts("Square");
				}
				//causes massive input lag here! ~ should be fixed with eventfilter
				else if (event.type == SDL_CONTROLLERAXISMOTION)
				{
					int maxValue = 32767;

					//if analogInput is accepted, process the event!
					if (analogInput.returnCode == 1)
					{
						//printf("Processed analog input: X: %d | Y: %d\n", analogInput.xInput, analogInput.yInput);

						float unitX = (float)analogInput.xInput / maxValue;
						float unitY = (float)analogInput.yInput / maxValue;

						float unitX2 = (float)analogInput.rawX / maxValue;
						float unitY2 = (float)analogInput.rawY / maxValue;

						float scale = 100;

						processedInputPos = SDL_Point{ (int)(unitX * scale) + center.x, (int)(unitY * scale) + center.y };
					    rawInputPos = SDL_Point{ (int)(unitX2 * scale) + center.x, (int)(unitY2 * scale) + center.y };

						printf("ProcessInputPos: x: %d | y: %d\n", processedInputPos.x, processedInputPos.y);
						float unitDist = unitX * unitX + unitY * unitY;
						printf("unitX^2 + unitY^2 = %f\n", unitDist);
					}
					else if(analogInput.returnCode == 0)
					{
						processedInputPos = center;

						float scale = 100;
						float unitX2 = (float)analogInput.rawX / maxValue;
						float unitY2 = (float)analogInput.rawY / maxValue;
						rawInputPos = SDL_Point{ (int)(unitX2 * scale) + center.x, (int)(unitY2 * scale) + center.y };
					}
				}
			}
		}

		//frames drop by 10-15 when I draw these on screen.
		//DrawAnalogDisplay(&core);
		//DrawRawInput(&core, rawInputPos);
		//DrawProcessedInput(&core, processedInputPos);

		SDL_RenderPresent(core.getRenderer());
		SDL_RenderClear(core.getRenderer());

		//measure the amount of time it took to finish the update
		endCount = SDL_GetPerformanceCounter();
		Uint32 countElapsed = (Uint32)(endCount - startCount);
		observedDeltaTime = 1000.0f * countElapsed / performanceFrequency;
		observedFPS = (Uint32)performanceFrequency / countElapsed;
		currentTime += observedDeltaTime;

		float msDifference = core.getTargetDeltaTime() - observedDeltaTime;
		//if device finishes a cycle faster than targetDeltaTime, sleep for a bit;
		if (msDifference > 0)
		{
			Uint32 msToSleep = (Uint32)(observedDeltaTime + msDifference);
			SDL_Delay(msToSleep);
		}
		else //if device is taking longer to process due to OS scheduling, try to sleep enough to hit targetFPS - don't sleep at all possibly
		{
			SDL_Delay((Uint32)core.getTargetDeltaTime());
		}

		startCount = endCount;

		DisplayFPS(&core, observedFPS, currentTime, 100);
	}


	//CLEANUP
	if (controller != NULL)
		SDL_GameControllerClose(controller);

	return 0;
}

/* Params:
Core : obtains the window via core.getWindow();
observedFPS : the FPS obtained from the game loop
currentTime : time since game loop started
frequency : how often to display the observedFPS. Measured in milliseconds
*/
void DisplayFPS(Core* core, Uint32 observedFPS, const float currentTime, int frequency)
{
	if ((int)currentTime % frequency == 0)
	{
		std::string title("Controller Input Visual | FPS: ");
		title += std::to_string(observedFPS);
		SDL_SetWindowTitle(core->getWindow(), title.c_str());
	}
}

//note we can return the void* !!!
//This functin gets invoked every update via SDL_SetEventFilter();
int AnalogStickFilter(void* analogData, SDL_Event* event)
{
	AnalogData* analog = static_cast<AnalogData*>(analogData);
	if (analog == NULL)
	{
		printf("Something terribly went wrong with receiving analog stick input!\n");
		return 1;
	}

	//TODO: fine tune the code a bit... give the visual some interpolation!
	//convert to radial deadzone.
	if (event->type == SDL_CONTROLLERAXISMOTION)
	{
		
		int rawX = SDL_GameControllerGetAxis(analog->controller, SDL_CONTROLLER_AXIS_LEFTX);
		int rawY = SDL_GameControllerGetAxis(analog->controller, SDL_CONTROLLER_AXIS_LEFTY);
		
		float mag = SDL_sqrtf((float)(rawX * rawX) + (float)(rawY * rawY));

		//ignore analog inputs if the square magnitude of the raw input vector is within the inner deadzone.
		if ((float)mag / 32767 < (float)INNER_DEADZONE / 32767)
		{
			analog->rawX = rawX;
			analog->rawY = rawY;
			analog->returnCode = 0;
		}
		else
		{
			analog->rawX = rawX;
			analog->rawY = rawY;
			analog->xInput = rawX;
			analog->yInput = rawY;
			analog->returnCode = 1;
		}
		
		//printf("Raw AnalogStick Input X: %d | Y: %d\n", rawX, rawY);
		return analog->returnCode;
	}

	//reset analog values
	analog->rawX = 0;
	analog->rawY = 0;
	analog->xInput = 0;
	analog->yInput = 0;

	//process all other events
	return 1;
}

/** Drawing a circle
	https://forums.libsdl.org/viewtopic.php?p=46027
**/

void RenderCircle(Core* core, const SDL_Point& center, float radius, const SDL_Color& color, int sides)
{
	if (sides == 0)
		sides = 2 * M_PI / 2;

	float d_a = 2 * M_PI / sides;//angle delta.
	float angle = d_a;

	SDL_Point start, end;
	end.x = radius;
	end.y = 0.0f;
	end.x += center.x;
	end.y += center.y;

	for (int i = 0; i != sides; ++i)
	{
		start = end;
		end.x = cos(angle) * radius;
		end.y = sin(angle) * radius;
		end.x += center.x;
		end.y += center.y;
		angle += d_a;
		RenderLine(core, start, end, color);
	}
}

void RenderLine(Core* core, const SDL_Point start, const SDL_Point end, const SDL_Color color)
{
	SDL_SetRenderDrawColor(core->getRenderer(), color.r, color.g, color.b, color.a);

	//draw line
	SDL_RenderDrawLine(core->getRenderer(), start.x, start.y, end.x, end.y);
}

void DrawAnalogDisplay(Core * core)
{
	//gamecontroller analog stick input visualization in a function for game loop to use.
	//set bg color to grey
	SDL_SetRenderDrawColor(core->getRenderer(), 100, 100, 100, 255);
	SDL_RenderClear(core->getRenderer());

	SDL_Rect boxOutline;
	boxOutline.h = SCREEN_HEIGHT - 1;
	boxOutline.w = SCREEN_WIDTH - 1;
	boxOutline.x = 0;
	boxOutline.y = 0;
	SDL_SetRenderDrawColor(core->getRenderer(), 0, 0, 0, 255);
	SDL_RenderDrawRect(core->getRenderer(), &boxOutline);

	//vertical line - y axis
	SDL_Point s1{ SCREEN_WIDTH / 2, 0 };
	SDL_Point e1{ SCREEN_WIDTH / 2, SCREEN_HEIGHT - 1 };
	SDL_RenderDrawLine(core->getRenderer(), s1.x, s1.y, e1.x, e1.y);


	//horizontal line - x axis
	SDL_Point s2{ 0, SCREEN_HEIGHT / 2 };
	SDL_Point e2{ SCREEN_WIDTH - 1, SCREEN_HEIGHT / 2 };
	SDL_RenderDrawLine(core->getRenderer(), s2.x, s2.y, e2.x, e2.y);


	//draw unit circle
	SDL_Point center{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	SDL_Color color;
	color.a = 255;
	color.r = 255;
	color.g = 0;
	color.b = 0;
	RenderCircle(core, center, 100.0f, color, 500);

	//draw outer deadzone;
	color.r = 0;
	color.g = 0;
	color.b = 255;
	RenderCircle(core, center, 90.0f, color, 500);

	//draw inner deadzone;
	RenderCircle(core, center, 25.0f, color, 500);
}

void DrawProcessedInput(Core* core, const SDL_Point newPos)
{
	//draw processed analog input white
	SDL_Color color;
	color.r = 255; color.g = 255; color.b = 255; color.a = 255;
	float radius = 4.0f;
	RenderCircle(core, newPos, radius, color, 10);
}
void DrawRawInput(Core* core, const SDL_Point newPos)
{
	//draw raw analog input green
	SDL_Color color;
	color.r = 0; color.g = 255; color.b = 0; color.a = 255;
	float radius = 2.0f;
	RenderCircle(core, newPos, radius, color, 10);
}








