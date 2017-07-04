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

		pressedTime = 0;
		timeHeldDelta = 300;
	}
}


//there will be 3 button states
//pressed - button press lasts no longer than 1000
//held - button press lasts longer than 1000
//released - button is let go of

void GameControllerComponent::Update(float deltaTime)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_CONTROLLERBUTTONUP)
		{
			int heldTimeDuration = event.cbutton.timestamp - pressedTime;
			printf("Button Up: %d\n",event.cbutton.timestamp);
			printf("HeldTimeDuration: %d\n", heldTimeDuration);
			//buttonMap["A"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
			//buttonMap["B"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
			//buttonMap["X"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
			//buttonMap["Y"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);

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
			//buttonMap["A"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
			//buttonMap["B"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B);
			//buttonMap["X"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X);
			//buttonMap["Y"] = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y);

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
		else if(event.cbutton.timestamp - pressedTime > timeHeldDelta)
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

