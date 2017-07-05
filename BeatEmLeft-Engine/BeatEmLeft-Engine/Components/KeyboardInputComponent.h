#ifndef KEYBOARDINPUTCOMPONENT_H
#define KEYBOARDINPUTCOMPONENT_H
#include "../Systems/Component.h"
#include <unordered_map>

enum class KeyStates
{
	PRESSED,
	HELD,
	RELEASED
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

