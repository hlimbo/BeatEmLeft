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

	elapsedTime["left"] = 0.0f;
	elapsedTime["right"] = 0.0f;
	elapsedTime["up"] = 0.0f;
	elapsedTime["down"] = 0.0f;
	elapsedTime["a"] = 0.0f;
	elapsedTime["s"] = 0.0f;
	elapsedTime["d"] = 0.0f;
	elapsedTime["space"] = 0.0f;
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

//the subtle difference between gamepad controller events
//and key events is that key events get checked every update
//e.g. if key is down for more than 1 frame, it will continue to 
//process that input as a key down again...
void KeyboardController::HandleInput(const SDL_Event& event)
{
	//event.key.repeat only records the most recently pressed key...
	//so if you hold down more than 2 keys at once it only registers the last key held
	if (event.type == SDL_KEYDOWN)
	{
		SDL_Keycode keycode = event.key.keysym.sym;
		//startPressedTime = event.key.timestamp
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
	
	if (event.type == SDL_KEYUP)
	{
		//puts("key released");
		SDL_Keycode keycode = event.key.keysym.sym;

		switch (keycode)
		{
		case SDLK_UP:
			keys["up"] = KeyStates::RELEASED;
			elapsedTime["up"] = 0.0f;
			break;
		case SDLK_DOWN:
			keys["down"] = KeyStates::RELEASED;
			elapsedTime["down"] = 0.0f;
			break;
		case SDLK_LEFT:
			keys["left"] = KeyStates::RELEASED;
			elapsedTime["left"] = 0.0f;
			break;
		case SDLK_RIGHT:
			keys["right"] = KeyStates::RELEASED;
			elapsedTime["right"] = 0.0f;
			break;
		case SDLK_a:
			keys["a"] = KeyStates::RELEASED;
			elapsedTime["a"] = 0.0f;
			break;
		case SDLK_s:
			keys["s"] = KeyStates::RELEASED;
			elapsedTime["s"] = 0.0f;
			break;
		case SDLK_d:
			keys["d"] = KeyStates::RELEASED;
			elapsedTime["d"] = 0.0f;
			break;
		case SDLK_SPACE:
			keys["space"] = KeyStates::RELEASED;
			elapsedTime["space"] = 0.0f;
			break;
		}
	}
}

void KeyboardController::UpdateKeyStates(float deltaTime)
{

	//update time
	for (auto it = keys.begin();it != keys.end();++it)
	{
		if (it->second == KeyStates::PRESSED || it->second == KeyStates::HELD)
		{
			elapsedTime[it->first] += deltaTime;
		}
	}

	//update state from pressed to held
	for (auto it = elapsedTime.begin();it != elapsedTime.end();++it)
	{
		if (keys[it->first] == KeyStates::PRESSED && it->second > latency)
		{
			//printf("button held: %s\n", it->first.c_str());
			keys[it->first] = KeyStates::HELD;
		}
	}
}
