#ifndef ANIMATION_STATE_MACHINE_H
#define ANIMATION_STATE_MACHINE_H

#include "../Components/Animation.h"
#include "../Components/SpriteSheet.h"
#include "../Components/Transform.h"
#include "../Components/Kinematic.h"


//this class won't be a generic class that can be utilized
//for any entity that contains an Animation component since
//there could be different names assigned to the animation states
//and there could be different conditions for each entity that 
//invoke the transition from one state to the next.

//temp
using namespace std;

//this class will decide which spritesheet animations
//to use from the animation component depending on other components data
// e.g. if velocity.x > 0 then go from idle animation to running animation
class AnimationStateMachine
{
public:
	AnimationStateMachine(Animation* anim)
	{
		playerAnimation = anim;
	}
	~AnimationStateMachine() {}

	//initializes the relevant components (data) required to change animation state
	void InitComponents(Transform* transform, Kinematic* kinematic)
	{
		playerTransform = transform;
		playerKinematic = kinematic;
	}

	//returns the animation to play when certain conditions are met
	SpriteSheet* GetAnimation()
	{
		//heart of the state machine
		SpriteSheet* sheet = nullptr;

		if (playerKinematic->velocity.x > 0.0f && playerKinematic->velocity.y == 0.0f)
		{
			sheet = playerAnimation->Get("run");
		}

		if (playerKinematic->velocity.x == 0.0f)
		{
			sheet = playerAnimation->Get("Idle");
		}


		return sheet;
	}


private:
	//temp name
	Animation* playerAnimation;
	Transform* playerTransform;
	Kinematic* playerKinematic;
};


#endif