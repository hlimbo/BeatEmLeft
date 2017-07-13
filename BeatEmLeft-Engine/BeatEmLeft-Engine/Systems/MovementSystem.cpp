#include "MovementSystem.h"
#include "ECS.h"
#include "EntitySystem.h"
#include "../Components/Transform.h"
#include "../Components/Kinematic.h"
#include "../Input/KeyboardController.h"
#include <vector>

using namespace std;

//Note: speed != velocity (velocity = speed * direction)
//custom functionality of how stuff should accelerate
float accelerate_in(float currentSpeed, float maxSpeed, float deltaTime, float deltaSpeed)
{
	return currentSpeed > maxSpeed ? maxSpeed : currentSpeed + (currentSpeed * deltaSpeed * deltaTime);
}

MovementSystem::MovementSystem(ECS* ecs)
{
	this->ecs = ecs;
	transforms = &(ecs->transforms);
	kinematics = &(ecs->kinematics);
}

void MovementSystem::Init()
{
	//temp code
	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	keyboard = ecs->GetKeyboard(playerID);
}


//something to think about... should kinematic movement decide
//which spritesheet animations to play or do button presses decide
//which spritesheet animations to play

//use this to play around with speed,velocity,acceleration,and direction based
//on the user input states.
void MovementSystem::UpdateKinematics(float deltaTime)
{
	//temp code
	//int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	//keyboard = ecs->GetKeyboard(playerID);
	//Kinematic* kinematic = ecs->kinematics.GetComponent(playerID);
	//Transform* transform = ecs->transforms.GetComponent(playerID);

	//this loop is what allow flexibility of the system
	//that is whenever I register keyboard control to another entity
	//the amount of effort to do so is minimal!
	vector<int> entityIDs = ecs->entitySystem.GetIDs();
	for (vector<int>::iterator it = entityIDs.begin();it != entityIDs.end();++it)
	{
		keyboard = ecs->GetKeyboard(*it);
		Kinematic* kinematic = ecs->kinematics.GetComponent(*it);
		Transform* transform = ecs->transforms.GetComponent(*it);
		if (keyboard != nullptr && transform != nullptr && kinematic != nullptr)
		{
			if (keyboard->KeyReleased("left") &&
				keyboard->KeyReleased("right") &&
				keyboard->KeyReleased("up") &&
				keyboard->KeyReleased("down"))
			{
				kinematic->direction = Vect2(0.0f, 0.0f);
				kinematic->currentSpeed = kinematic->minSpeed;
			}
			else if ((keyboard->KeyPressed("left") && keyboard->KeyPressed("right")) ||
				(keyboard->KeyHeld("left") && keyboard->KeyHeld("right")) ||
				(keyboard->KeyPressed("left") && keyboard->KeyHeld("right")) ||
				(keyboard->KeyHeld("left") && keyboard->KeyPressed("right")))
			{
				//don't move the player if that is the case
				kinematic->direction.x = 0.0f;
				//this will be problematic when implementing jump
				//since if both <- and -> are pressed at the same time while in the air
				//falling will feel floaty (y velocity does not accelerate downwards
				//and instead y velocity remains constant)
				kinematic->currentSpeed = kinematic->minSpeed;
			}
			else
			{
				//to compensate for the half a second lag from transitioning from key pressed to held
				//I will apply some acceleration to both states.

				//a lot more complicated than having only 2 keyboard states released
				//or pressed this is because I want precise button presses for the game
				//will make it into a fighting esque like game.

				float ds = 1.7f;//how much should velocity change per update
				if (keyboard->KeyPressed("left"))
				{
					kinematic->direction.x = -1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.x = -kinematic->currentSpeed * deltaTime;
				}
				else if (keyboard->KeyHeld("left"))
				{
					kinematic->direction.x = -1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.x = -kinematic->currentSpeed * deltaTime;
				}

				if (keyboard->KeyPressed("right"))
				{
					kinematic->direction.x = 1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.x = kinematic->currentSpeed * deltaTime;
				}
				else if (keyboard->KeyHeld("right"))
				{
					kinematic->direction.x = 1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.x = kinematic->currentSpeed * deltaTime;
				}

				if (keyboard->KeyPressed("up"))
				{
					kinematic->direction.y = -1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.y = -kinematic->currentSpeed * deltaTime;
				}
				else if (keyboard->KeyHeld("up"))
				{
					kinematic->direction.y = -1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.y = -kinematic->currentSpeed * deltaTime;
				}

				if (keyboard->KeyPressed("down"))
				{
					kinematic->direction.y = 1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.y = kinematic->currentSpeed * deltaTime;
				}
				else if (keyboard->KeyHeld("down"))
				{
					kinematic->direction.y = 1.0f;
					kinematic->currentSpeed = accelerate_in(kinematic->currentSpeed, kinematic->maxSpeed, deltaTime, ds);
					//kinematic->velocity.y = kinematic->currentSpeed * deltaTime;
				}

				//temp code handling diagonals (only applies to keyboard)
				//up-left
				if (kinematic->direction.x == -1.0f && kinematic->direction.y == -1.0f)
				{
					kinematic->direction = Vect2(cosf(5 * (float)M_PI / 4), sinf(-(float)M_PI / 4));
				}
				//up-right
				else if (kinematic->direction.x == 1.0f && kinematic->direction.y == -1.0f)
				{
					kinematic->direction = Vect2(cosf((float)M_PI / 4), sinf(-(float)M_PI / 4));
				}
				//down-left
				else if (kinematic->direction.x == -1.0f && kinematic->direction.y == 1.0f)
				{
					kinematic->direction = Vect2(cosf(5 * (float)M_PI / 4), sinf((float)M_PI / 4));
				}
				//down-right
				else if (kinematic->direction.x == 1.0f && kinematic->direction.y == 1.0f)
				{
					kinematic->direction = Vect2(cosf((float)M_PI / 4), sinf((float)M_PI / 4));
				}

			}

		}
	}

}

void MovementSystem::UpdatePositions(float deltaTime)
{

	//this is all what we want to do here!
	vector<int> entityIDs = ecs->entitySystem.GetIDs();
	for (vector<int>::iterator it = entityIDs.begin();it != entityIDs.end();++it)
	{
		Transform* transform = transforms->GetComponent(*it);
		Kinematic* kinematic = kinematics->GetComponent(*it);

		//should probably copy new position to go to in the boxCollider's position
		//first..
		if (transform != nullptr && kinematic != nullptr)
		{
			//temp code ~ if acceleration is zero..this means that velocity will be constant
			kinematic->velocity = kinematic->direction * kinematic->currentSpeed* deltaTime;
			transform->position = transform->position + kinematic->velocity;
		}
	}
}
