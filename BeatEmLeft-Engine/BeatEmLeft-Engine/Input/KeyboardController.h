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

class KeyboardController
{
public:
	KeyboardController() {}
	~KeyboardController() {}

	//will take in a file later on.......this should be dealt with towards the end of the engine..
	void Init();
	void HandleInput(const SDL_Event& event);


	bool KeyPressed(std::string keyName);
	bool KeyHeld(std::string keyName);
	bool KeyReleased(std::string keyName);
private:
	std::unordered_map<std::string, KeyStates> keys;
	
	//debug variables
	int startPressedTime;
	int counter;
};

#endif

