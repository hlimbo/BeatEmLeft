#define MAX_VALUE 32768
//#define DEADZONE 1639
#define DEADZONE 4915
#include "GameController.h"
#include "SDL_error.h"
#include "SDL_log.h"
#include <math.h>

#include <iostream>
using namespace std;

//TODO: probably in the future, I need to make ComponentSystems a base class
//that can be inherited from
//Most likely will need a GameControllerComponentSystem that manages:
//1. when a controller is connected to computer
//2. when a controller is disconnected from computer
//3. keeps track of controller_ids
//4. keeps track of how many controllers are attached to the computer.
//5. (extra) handles custom input mapping if I want this feature for my game..
void GameController::Init()
{
	//int numJoySticks = SDL_NumJoysticks();

	//TODO: Add support for multiple controllers taht can be used.
	//for now assume only one controller can be attached to the computer
	controller = SDL_GameControllerOpen(0);
	if (controller == NULL)
		SDL_Log("Error: Controller: %s\n", SDL_GetError());
	else
	{
		//SDL_Log("%s\n",SDL_GameControllerMapping(controller));
		
		//TODO: Read from SDL_GameControllerMapping string
		//	    Extract buttonMappings supplied in the string (parse it prob by using strtok)
		//		Use each buttonmapping in the string as a key to the buttonMap data structure
		buttonMap["A"] = ButtonStates::RELEASED;
		buttonMap["B"] = ButtonStates::RELEASED;
		buttonMap["X"] = ButtonStates::RELEASED;
		buttonMap["Y"] = ButtonStates::RELEASED;

		pressedTime = 0;
		timeHeldDelta = 300;
	}
}


//there will be 3 button states
//pressed - button press lasts no longer than 1000
//held - button press lasts longer than 1000
//released - button is let go of

void GameController::HandleInput(const SDL_Event& event)
{
	if (event.type == SDL_CONTROLLERBUTTONUP)
	{
		int heldTimeDuration = event.cbutton.timestamp - pressedTime;
		printf("Button Up: %d\n",event.cbutton.timestamp);
		printf("HeldTimeDuration: %d\n", heldTimeDuration);


		if (!SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
		{
			buttonMap["A"] = ButtonStates::RELEASED;
			//puts("A released");
		}
		if (!SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
		{
			buttonMap["B"] = ButtonStates::RELEASED;
			//puts("B released");
		}
		if (!SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X))
		{
			buttonMap["X"] = ButtonStates::RELEASED;
			//puts("X released");
		}
		if (!SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y))
		{
			buttonMap["Y"] = ButtonStates::RELEASED;
			//puts("Y released");
		}
	}

	if (event.type == SDL_CONTROLLERBUTTONDOWN)
	{
		pressedTime = event.cbutton.timestamp;
		printf("Button Down: %d\n", event.cbutton.timestamp);

		if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
		{
			buttonMap["A"] = ButtonStates::PRESSED;
			puts("A pressed");
		}
		if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
		{
			buttonMap["B"] = ButtonStates::PRESSED;
			puts("B pressed");
		}
		if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X))
		{
			buttonMap["X"] = ButtonStates::PRESSED;
			puts("X pressed");
		}
		if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y))
		{
			buttonMap["Y"] = ButtonStates::PRESSED;
			puts("Y pressed");
		}
	}
	else if((int)(event.cbutton.timestamp) - pressedTime > timeHeldDelta)
	{
		if (buttonMap["A"] == ButtonStates::PRESSED)
		{
			buttonMap["A"] = ButtonStates::HELD;
			puts("A held");
		}

		if (buttonMap["B"] == ButtonStates::PRESSED)
		{
			buttonMap["B"] = ButtonStates::HELD;
			puts("B held");
		}

		if (buttonMap["X"] == ButtonStates::PRESSED)
		{
			buttonMap["X"] = ButtonStates::HELD;
			puts("X held");
		}

		if (buttonMap["Y"] == ButtonStates::PRESSED)
		{
			buttonMap["Y"] = ButtonStates::HELD;
			puts("Y held");
		}
	}

	if (event.type == SDL_CONTROLLERAXISMOTION)
	{
		//raw left and right analog stick inputs
		int lx= SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
		int ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
		int rx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
		int ry = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
	
		//Inner Deadzones
		//adjust for deadzones 1639 / 32768 is about 5%  inner deadzone
		leftAnalogInput = adjustAnalogInput(lx, ly);
		rightAnalogInput = adjustAnalogInput(rx, ry);

		//printf("leftAnalogInput (%f, %f)\n", leftAnalogInput.x, leftAnalogInput.y);
		//printf("rightAnalogInput (%f, %f)\n", rightAnalogInput.x, rightAnalogInput.y);
	}
}

bool GameController::ButtonPressed(std::string buttonName)
{
	return buttonMap.at(buttonName) == ButtonStates::PRESSED;
}

bool GameController::ButtonHeld(std::string buttonName)
{
	return buttonMap.at(buttonName) == ButtonStates::HELD;
}

bool GameController::ButtonReleased(std::string buttonName)
{
	return buttonMap.at(buttonName) == ButtonStates::RELEASED;
}

Vect2 GameController::LeftAnalogInput()
{
	return leftAnalogInput;
}

Vect2 GameController::RightAnalogInput()
{
	return rightAnalogInput;
}

//ax = raw analog input value along x axis
//ay = raw analog input value along y axis
Vect2 GameController::adjustAnalogInput(int ax, int ay)
{
	float ux, uy;
	ux = (ax < -DEADZONE || ax > DEADZONE) ? (float)ax / MAX_VALUE : 0.0f;
	uy = (ay < -DEADZONE || ay > DEADZONE) ? (float)ay / MAX_VALUE : 0.0f;

	//make sure analog axis values max out to 1.0f when the other axis is at 0.0f
	//Not sure if this is needed... might remove later on...
	//float tolerance = 0.001f;
	//if (ux == 0.0f && fabsf(fabsf(uy) - 1.0f) < tolerance)
	//	uy = (uy < 0.0f) ? -1.0f : 1.0f;

	//if (uy == 0.0f && fabsf(fabsf(ux) - 1.0f) < tolerance)
	//	ux = (ux < 0.0f) ? -1.0f : 1.0f;

	return Vect2(ux, uy);
}

