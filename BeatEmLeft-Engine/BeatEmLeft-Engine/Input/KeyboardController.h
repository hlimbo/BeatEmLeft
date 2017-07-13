#ifndef KEYBOARD_CONTROLLER_H
#define KEYBOARD_CONTROLLER_H
#include <unordered_map>
#include <SDL2/SDL_events.h>

/*
//TODO(Harvey):
Rename to KeyboardController which takes in 2 methods
	1. Init(const char* configFilePath); 
		a. opens a configuration file
		b. loads in the supported button mappings for the application
*/

enum class KeyStates
{
	PRESSED, //this state means the key was pressed in one frame (current frame).
	HELD, //this state means the key was pressed more than one frame
	RELEASED, //this state means the key was released in one frame or more frames(current frame).
};

//used as a way to identify what kind of event triggered the input
enum class InputEventType
{
	KEYDOWN,  //equivalent to SDL_KEYDOWN
	KEYREPEAT, //equivalent to SDL_KEYDOWN && event.key.repeat != 0;
	KEYUP, //equivalent to SDL_KEYUP
	UNDEFINED //used to ignore events from SDL_Event we don't care about
};

class KeyboardController
{
public:
	KeyboardController() { inputType = InputEventType::UNDEFINED; }
	~KeyboardController() {}

	//will take in a file later on.......this should be dealt with towards the end of the engine..
	void Init();
	void HandleInput(const SDL_Event& event);


	bool KeyPressed(std::string keyName);
	bool KeyHeld(std::string keyName);
	bool KeyReleased(std::string keyName);
	InputEventType GetInputType();
private:
	std::unordered_map<std::string, KeyStates> keys;
	//this gets updated every time handleInput is called.
	//can be set to UNDEFINED if no new key input events were triggered
	InputEventType inputType;

	//debug variables
	int startPressedTime;
	int counter;
};

#endif

