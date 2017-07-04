#ifndef GAMECONTROLLERCOMPONENT_H
#define GAMECONTROLLERCOMPONENT_H
#include "../Systems/Component.h"
#include "SDL2/SDL_gamecontroller.h"
#include <unordered_map>

enum class ButtonStates
{
	RELEASED,
	PRESSED,
	HELD
};

class GameControllerComponent :
	public Component
{
public:
	GameControllerComponent(std::string type) : Component(type) {}
	virtual ~GameControllerComponent() {}

	virtual void Init();
	virtual void Update(float deltaTime);

	bool buttonPressed(std::string buttonName);
	bool buttonHeld(std::string buttonName);
	bool buttonReleased(std::string buttonName);
private:
	SDL_GameController* controller;
	std::unordered_map<std::string, ButtonStates> buttonMap;

	int releasedTime;
	int pressedTime;
};

#endif

