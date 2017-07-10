#ifndef KEYBOARDINPUTCOMPONENT_H
#define KEYBOARDINPUTCOMPONENT_H
#include "../Systems/Component.h"
#include <unordered_map>

/*
//TODO(Harvey): stop inheriting from component
Rename to KeyboardController which takes in 2 methods
	1. Init(const char* configFilePath); 
		a. opens a configuration file
		b. loads in the supported button mappings for the application
	2. HandleInput(const SDL_Event& event);
		a. takes in a const reference (which in c world, a reference = const pointer)
		b. processes supported input events for the game 
			i. convert events such as key up and key down into 4 different key states
				other components or systems can use:
		c. Convert key up and key down events into 4 different states
			1. key released:  key was just released this frame (1 frame)
			2. key pressed: key was just pressed this frame (1 frame)
			3. key held: key was pressed more than 1 frame
			4. key na: key was released more than one frame
*/

enum class KeyStates
{
	PRESSED, //this state means the key was pressed in one frame (current frame).
	HELD, //this state means the key was pressed more than one frame
	RELEASED, //this state means the key was released in one frame (current frame).
	NA //this state means the key was released more than one frame.
};

class KeyboardInputComponent :
	public Component
{
public:
	KeyboardInputComponent(std::string type) : Component(type) {}
	virtual ~KeyboardInputComponent() {}

	virtual void Init();
	virtual void Update(float deltaTime);

	bool KeyPressed(std::string keyName);
	bool KeyHeld(std::string keyName);
	bool KeyReleased(std::string keyName);
private:
	std::unordered_map<std::string, KeyStates> keys;
	int startPressedTime;
	
	int counter;
};

#endif

