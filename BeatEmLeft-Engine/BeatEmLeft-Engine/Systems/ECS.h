//Entity-Component System (ECS)
/*
	Contains:
		1. a single EntitySystem
		2. multiple ComponentManagers

	EntitySystem and ComponentManagers are combined to centralize Initialization and Update logic
	of components. EntitySystem is part of this system because components 
	need to associate themselves with an entity's id. This system is an alternative
	to the object-oriented/inheritance approach of creating a game engine.

	Advantages:
		1. Code will be more maintainable.
		(the larger this project grows, the more apparent this benefit will be a plus)
		2. Easy to keep track of the entity and the components it has since I only need
		to look at the entity's id.

	Disadvantages(Maybe):
		1. probably hard to rapidly design a game mechanic as it involves world modeling.
		Note: creating a mental model of the game world naturally causes me to think in an object oriented approach
		as opposed to a data oriented approach.
*/
#ifndef ECS_H
#define ECS_H

#include "EntitySystem.h"
#include "ComponentManager.h"
#include "../Components/Sprite.h"
#include "../Components/Transform.h"
#include "../Components/Kinematic.h"
#include "../Input/KeyboardController.h"

//this is a much nicer way of looking at the managers and system 
//at a very high level!
struct ECS
{
	EntitySystem entitySystem;

	ComponentManager<Sprite> sprites;
	ComponentManager<Transform> transforms;
	ComponentManager<Kinematic> kinematics;

//Keyboard Interface-------------------------------------------------------------
	void InitKeyboard()
	{
		keyboard.first = -1;
		keyboard.second = new KeyboardController();
		keyboard.second->Init();
	}	
	KeyboardController* RegisterKeyboard(int entityID)
	{
		if (keyboard.first == -1)
		{
			keyboard.first = entityID;
			return keyboard.second;
		}
		
		return nullptr;//if keyboard is already registered to an entity
	}

	KeyboardController* GetKeyboard(int entityID)
	{
		if (entityID == keyboard.first)
			return keyboard.second;
		return nullptr;
	}

	void UnregisterKeyboard(int entityID)
	{
		if (keyboard.first != -1)
		{
			keyboard.first = -1;
		}
	}

	void FreeKeyboard()
	{
		if (keyboard.second != nullptr)
		{
			delete keyboard.second;
			keyboard.second = nullptr;
		}
	}

private:
	std::pair<int, KeyboardController*> keyboard;
};

#endif

/*
	what this class would look like without component managers
	struct ECS
	{
		EntitySystem entitySystem;

		ComponentManager<Sprite> sprites;
		ComponentManager<SpriteSheet> spriteSheets;
		ComponentManager<Transform> transforms;
		ComponentManager<Kinematic> kinematics;
		ComponentManager<GameController> gameControllers;
		
		std::pair<int,KeyboardController*> keyboard;
		KeyboardController* CreateKeyboard();
		RegisterEntityToKeyboard(int id);
		UnRegisterEntityFromKeyboard(int id);
		FreeKeyboard(int id);
		FreeKeyboard(Keyboard* keyboard);

	}

	//written probably last..
	InputEventSystem inputEventSystem(
		ComponentManager<GameController>* controllers,
		std::pair<int,KeyboardController*>* keyboard);

	MovementSystem movementSystem(
		ComponentManager<GameController>* controllers,
		std::pair<int,KeyboardController*>* keyboard,
		ComponentManager<Transform>* transforms,
		ComponentManager<Kinematic>* kinematics);

	CollisionSystem collisionSystem(
		ComponentManager<Transform>* transforms,
		ComponentManager<Kinematic>* kinematics,
		ComponentManager<BoxCollider>* boxColliders);
	
	RenderSystem renderSystem(
		ComponentManager<Transform>* transforms,
		ComponentManager<Sprite>* sprites);


*/

