#define MAX_VALUE 32768
#define DEADZONE 1639
#include "GameControllerComponent.h"
#include "SDL2/SDL_error.h"
#include "SDL2/SDL_log.h"
#include "SDL2/SDL_events.h"

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
void GameControllerComponent::Init()
{
	//int numJoySticks = SDL_NumJoysticks();

	//for now assume only one controller can be attached to the computer
	controller = SDL_GameControllerOpen(0);
	if (controller == NULL)
		SDL_Log("Error: Controller: %s\n", SDL_GetError());
	else
	{
		//SDL_Log("%s\n",SDL_GameControllerMapping(controller));
		
		buttonMap["A"] = ButtonStates::RELEASED;
		buttonMap["B"] = ButtonStates::RELEASED;
		buttonMap["X"] = ButtonStates::RELEASED;
		buttonMap["Y"] = ButtonStates::RELEASED;

		releasedTime = pressedTime = 0;
	}
}


//there will be 3 button states
//pressed - button press lasts no longer than 1000
//held - button press lasts longer than 1000
//released - button is let go of

void GameControllerComponent::Update(float deltaTime)
{
	if (buttonMap["A"] == ButtonStates::PRESSED)
	{
		buttonMap["A"] = ButtonStates::HELD;
	}

	if (buttonMap["B"] == ButtonStates::PRESSED)
	{
		buttonMap["B"] = ButtonStates::HELD;
	}

	if (buttonMap["X"] == ButtonStates::PRESSED)
	{
		buttonMap["X"] = ButtonStates::HELD;
	}

	if (buttonMap["Y"] == ButtonStates::PRESSED)
	{
		buttonMap["Y"] = ButtonStates::HELD;
	}

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_CONTROLLERBUTTONUP)
		{
			int releasedTimeDelta = event.cbutton.timestamp - pressedTime;
			releasedTime = event.cbutton.timestamp;
			printf("Button Up: %d\n",event.cbutton.timestamp);
			printf("ReleasedTimeDelta: %d\n", releasedTimeDelta);
			//buttonMap["A"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
			//buttonMap["B"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
			//buttonMap["X"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
			//buttonMap["Y"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);

			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
			{
				buttonMap["A"] = ButtonStates::RELEASED;
			}
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
			{
				buttonMap["B"] = ButtonStates::RELEASED;
			}
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X))
			{
				buttonMap["X"] = ButtonStates::RELEASED;
			}
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y))
			{
				buttonMap["Y"] = ButtonStates::RELEASED;
			}
		}

		if (event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			pressedTime = event.cbutton.timestamp;
			printf("Button Down: %d\n", event.cbutton.timestamp);
			//buttonMap["A"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
			//buttonMap["B"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
			//buttonMap["X"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
			//buttonMap["Y"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);

			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
			{
				buttonMap["A"] = ButtonStates::PRESSED;
			}
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
			{
				buttonMap["B"] = ButtonStates::PRESSED;
			}
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X))
			{
				buttonMap["X"] = ButtonStates::PRESSED;
			}
			if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y))
			{
				buttonMap["Y"] = ButtonStates::PRESSED;
			}
		}


	}
}

bool GameControllerComponent::buttonPressed(std::string buttonName)
{
	return buttonMap.at(buttonName) == ButtonStates::PRESSED;
}

bool GameControllerComponent::buttonHeld(std::string buttonName)
{
	return buttonMap.at(buttonName) == ButtonStates::HELD;
}

bool GameControllerComponent::buttonReleased(std::string buttonName)
{
	return buttonMap.at(buttonName) == ButtonStates::RELEASED;
}

