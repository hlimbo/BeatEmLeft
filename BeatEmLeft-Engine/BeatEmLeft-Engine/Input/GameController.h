#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H
#include "SDL_gamecontroller.h"
#include "SDL_events.h"
#include <unordered_map>
#include <Vect2.h>

enum class ButtonStates
{
	RELEASED,
	PRESSED,
	HELD
};

class GameController
{
public:
	GameController(){}
	~GameController() {}

	void Init();
	void HandleInput(const SDL_Event& event);

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

