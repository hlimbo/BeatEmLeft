#ifndef KEYBOARD_CONTROLLER_H
#define KEYBOARD_CONTROLLER_H
#include <unordered_map>
#include <SDL_events.h>

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
	//about 3 frames of keyboard delay from transitioning between pressed to held state
	KeyboardController() { latency = 55.7f;}
	KeyboardController(float latency) { this->latency = latency; }
	~KeyboardController() {}

	//will take in a file later on.......this should be dealt with towards the end of the engine..
	void Init();
	void HandleInput(const SDL_Event& event);
	//use this for handling between pressed and held keys
	//Usage:call this method after HandleInput, outside of the while(SDL_PollEvent(&event)) loop
	void UpdateKeyStates(float deltaTime);

	bool KeyPressed(std::string keyName);
	bool KeyHeld(std::string keyName);
	bool KeyReleased(std::string keyName);

	inline float GetLatency() { return latency; }

	//debug
	void PrintKeyStatuses();

private:
	std::unordered_map<std::string, KeyStates> keys;
	std::unordered_map<std::string, float> elapsedTime;
	
	//latency is the amount of time required in milliseconds to change the keyboard
	//state from pressed to held
	//the longer the latency, the less responsive the keyboard input gets
	//the lower the latency, the more responsive the keyboard input gets
	//however, the latency value should never be less than or equal to the amount
	//of time it takes in milliseconds to update a frame as responsiveness of the keys
	//depends on the framerate of the game
	float latency;
};

#endif

