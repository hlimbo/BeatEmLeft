#ifndef GAMECONTROLLERCOMPONENT_H
#define GAMECONTROLLERCOMPONENT_H
#include "../Systems/Component.h"
#include "SDL2/SDL_gamecontroller.h"
#include <unordered_map>
#include <Vect2.h>

enum class ButtonStates
{
	RELEASED,
	PRESSED,
	HELD,
	NA
};

class GameControllerComponent :
	public Component
{
public:
	GameControllerComponent(std::string type) : Component(type) {}
	virtual ~GameControllerComponent() {}

	virtual void Init();
	virtual void Update(float deltaTime);

	//the state of each button press gets updated regularly via Update call
	//make sure to check for the status of the button on every update 
	bool ButtonPressed(std::string buttonName);
	bool ButtonHeld(std::string buttonName);
	bool ButtonReleased(std::string buttonName);

	Vect2 LeftAnalogInput();
	Vect2 RightAnalogInput();

private:
	SDL_GameController* controller;
	std::unordered_map<std::string, ButtonStates> buttonMap;
	Vect2 leftAnalogInput;
	Vect2 rightAnalogInput;

	int pressedTime;
	int timeHeldDelta;//the number of milliseconds required to consider a button press to be a button held down.
	
	Vect2 adjustAnalogInput(int ax, int ay);
};

#endif

