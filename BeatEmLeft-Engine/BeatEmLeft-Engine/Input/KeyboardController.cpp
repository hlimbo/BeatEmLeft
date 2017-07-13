#include "KeyboardController.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_keycode.h"


void KeyboardController::Init()
{
	//init keys I want to use for the game....
	//should probably read from some file what keys I want to bind to..
	//but this should be good enough for now..
	keys["left"] = KeyStates::RELEASED;
	keys["right"] = KeyStates::RELEASED;
	keys["up"] = KeyStates::RELEASED;
	keys["down"] = KeyStates::RELEASED;
	keys["a"] = KeyStates::RELEASED;
	keys["s"] = KeyStates::RELEASED;
	keys["d"] = KeyStates::RELEASED;
	keys["space"] = KeyStates::RELEASED;

	startPressedTime = 0;
	counter = 0;
}

bool KeyboardController::KeyPressed(std::string keyName)
{
	return keys.at(keyName) == KeyStates::PRESSED;
}

bool KeyboardController::KeyHeld(std::string keyName)
{
	return keys.at(keyName) == KeyStates::HELD;
}

bool KeyboardController::KeyReleased(std::string keyName)
{
	return keys.at(keyName) == KeyStates::RELEASED;
}

InputEventType KeyboardController::GetInputType()
{
	return inputType;
}


//the subtle difference between gamepad controller events
//and key events is that key events get checked every update
//e.g. if key is down for more than 1 frame, it will continue to 
//process that input as a key down again...
void KeyboardController::HandleInput(const SDL_Event& event)
{
	inputType = InputEventType::UNDEFINED;

	if (event.type == SDL_KEYDOWN)
	{
		SDL_Keycode keycode = event.key.keysym.sym;
		if (event.key.repeat == 0) // key pressed
		{
			inputType = InputEventType::KEYDOWN;
			//puts("key pressed!");
			startPressedTime = event.key.timestamp;
			switch (keycode)
			{
				//movement keys
			case SDLK_UP:
				keys["up"] = KeyStates::PRESSED;
				break;
			case SDLK_DOWN:
				keys["down"] = KeyStates::PRESSED;
				break;
			case SDLK_LEFT:
				keys["left"] = KeyStates::PRESSED;
				break;
			case SDLK_RIGHT:
				keys["right"] = KeyStates::PRESSED;
				break;
				//action keys
			case SDLK_a:
				keys["a"] = KeyStates::PRESSED;
				break;
			case SDLK_s:
				keys["s"] = KeyStates::PRESSED;
				break;
			case SDLK_d:
				keys["d"] = KeyStates::PRESSED;
				break;
			case SDLK_SPACE:
				keys["space"] = KeyStates::PRESSED;
				break;
			}
		}
		else //key held
		{
			inputType = InputEventType::KEYREPEAT;
			//puts("key held!");
			int inputLag = event.key.timestamp - startPressedTime;
			//printf("key pressed to held lag: %d\n", inputLag);//there is about a half a second input lag when transitioning from pressed to held...
			switch (keycode)
			{
				//movement keys
			case SDLK_UP:
				keys["up"] = KeyStates::HELD;
				break;
			case SDLK_DOWN:
				keys["down"] = KeyStates::HELD;
				break;
			case SDLK_LEFT:
				keys["left"] = KeyStates::HELD;
				break;
			case SDLK_RIGHT:
				keys["right"] = KeyStates::HELD;
				break;
				//action keys
			case SDLK_a:
				keys["a"] = KeyStates::PRESSED;
				break;
			case SDLK_s:
				keys["s"] = KeyStates::PRESSED;
				break;
			case SDLK_d:
				keys["d"] = KeyStates::PRESSED;
				break;
			case SDLK_SPACE:
				keys["space"] = KeyStates::PRESSED;
				break;
			}
		}

	}
	else if (event.type == SDL_KEYUP)
	{
		inputType = InputEventType::KEYUP;
		//puts("key released");
		SDL_Keycode keycode = event.key.keysym.sym;

		switch (keycode)
		{
		case SDLK_UP:
			keys["up"] = KeyStates::RELEASED;
			break;
		case SDLK_DOWN:
			keys["down"] = KeyStates::RELEASED;
			break;
		case SDLK_LEFT:
			keys["left"] = KeyStates::RELEASED;
			break;
		case SDLK_RIGHT:
			keys["right"] = KeyStates::RELEASED;
			break;
		case SDLK_a:
			keys["a"] = KeyStates::RELEASED;
			break;
		case SDLK_s:
			keys["s"] = KeyStates::RELEASED;
			break;
		case SDLK_d:
			keys["d"] = KeyStates::RELEASED;
			break;
		case SDLK_SPACE:
			keys["space"] = KeyStates::RELEASED;
			break;
		}
	}
}
