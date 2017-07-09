#include "KeyboardInputComponent.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_keycode.h"


void KeyboardInputComponent::Init()
{
	//init keys I want to use for the game....
	//should probably read from some file what keys I want to bind to..
	//but this should be good enough for now..
	keys["left"] = KeyStates::NA;
	keys["right"] = KeyStates::NA;
	keys["up"] = KeyStates::NA;
	keys["down"] = KeyStates::NA;
	keys["a"] = KeyStates::NA;
	keys["s"] = KeyStates::NA;
	keys["d"] = KeyStates::NA;
	keys["space"] = KeyStates::NA;

	startPressedTime = 0;
	counter = 0;
}

bool KeyboardInputComponent::KeyPressed(std::string keyName)
{
	return keys.at(keyName) == KeyStates::PRESSED;
}

bool KeyboardInputComponent::KeyHeld(std::string keyName)
{
	return keys.at(keyName) == KeyStates::HELD;
}

bool KeyboardInputComponent::KeyReleased(std::string keyName)
{
	return keys.at(keyName) == KeyStates::RELEASED;
}

//may need to rewrite this component since multiple functions that poll for events
//may have the information this keyboard component needs e.g. if I did not hit x to exit the application and have hit a keybinding instead,
//it may not record the keybinding in a separate event poll...

//the subtle difference between gamepad controller events
//and key events is that key events get checked every update
//e.g. if key is down for more than 1 frame, it will continue to 
//process that input as a key down again...
void KeyboardInputComponent::Update(float deltaTime)
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)//this statement will be true as long as a key is held down...
		{
			SDL_Keycode keycode = event.key.keysym.sym;
			//y-axis movement
			if (keycode == SDLK_UP)
			{
				if (keys["up"] == KeyStates::RELEASED)
					keys["up"] = KeyStates::PRESSED;
				else
					keys["up"] = KeyStates::HELD;
			}
			if (keycode == SDLK_DOWN)
			{
				if (keys["down"] == KeyStates::RELEASED)
					keys["down"] = KeyStates::PRESSED;
				else
					keys["down"] = KeyStates::HELD;
			}

			//x-axis movement
			if (keycode == SDLK_LEFT)
			{
				if (keys["left"] == KeyStates::RELEASED)
					keys["left"] = KeyStates::PRESSED;
				else
					keys["left"] = KeyStates::HELD;
			}
			if (keycode == SDLK_RIGHT)
			{
				if (keys["right"] == KeyStates::RELEASED)
					keys["right"] = KeyStates::PRESSED;
				else
					keys["right"] = KeyStates::HELD;
			}

			//action keys and space
			if (keycode == SDLK_a)
			{
				if (keys["a"] == KeyStates::RELEASED)
					keys["a"] = KeyStates::PRESSED;
				else
					keys["a"] = KeyStates::HELD;
			}
			if (keycode == SDLK_s)
			{
				if (keys["s"] == KeyStates::RELEASED)
					keys["s"] = KeyStates::PRESSED;
				else
					keys["s"] = KeyStates::HELD;
			}
			if (keycode == SDLK_d)
			{
				if (keys["d"] == KeyStates::RELEASED)
					keys["d"] = KeyStates::PRESSED;
				else
					keys["d"] = KeyStates::HELD;
			}
			if (keycode == SDLK_SPACE)
			{
				if (keys["space"] == KeyStates::RELEASED)
					keys["space"] = KeyStates::PRESSED;
				else
					keys["space"] = KeyStates::HELD;
			}

		}

		if (event.type == SDL_KEYUP)
		{
			SDL_Keycode keycode = event.key.keysym.sym;

			//y-axis movement
			if (keycode == SDLK_UP)
			{
				keys["up"] = KeyStates::RELEASED;
			}
			if (keycode == SDLK_DOWN)
			{
				keys["down"] = KeyStates::RELEASED;
			}

			//x-axis movement
			if (keycode == SDLK_LEFT)
			{
				keys["left"] = KeyStates::RELEASED;
			}
			if (keycode == SDLK_RIGHT)
			{
				keys["right"] = KeyStates::RELEASED;
			}

			//action keys and space
			if (keycode == SDLK_a)
			{
				keys["a"] = KeyStates::RELEASED;
			}
			if (keycode == SDLK_s)
			{
				keys["s"] = KeyStates::RELEASED;
			}
			if (keycode == SDLK_d)
			{
				keys["d"] = KeyStates::RELEASED;
			}
			if (keycode == SDLK_SPACE)
			{
				keys["space"] = KeyStates::RELEASED;
			}
		}
		else //if key was released for more than one frame set it back to NA
		{
			if (keys["up"] == KeyStates::RELEASED)
				keys["up"] = KeyStates::NA;
			if (keys["down"] == KeyStates::RELEASED)
				keys["down"] = KeyStates::NA;
			if (keys["left"] == KeyStates::RELEASED)
				keys["left"] = KeyStates::NA;
			if (keys["right"] == KeyStates::RELEASED)
				keys["right"] = KeyStates::NA;
		}
	}
}
