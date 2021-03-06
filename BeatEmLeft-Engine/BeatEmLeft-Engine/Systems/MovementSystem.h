#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "ComponentManager.h"
#include <Vect2.h>

struct ECS;
struct Kinematic;
struct Transform;
class KeyboardController;

class MovementSystem
{
public:
	MovementSystem(ECS* ecs);
	~MovementSystem() {}


	void Init();
	//deltaTime = in seconds
	void UpdateKinematics(float deltaTime);
	
	//temp functions for circle collisions
	void CheckForCircleCollisions(float deltaTimeInMS);

	//temp functions for AABB collisions
	void CheckForCollisions(float deltaTimeInMS);
	void CorrectCollisionOverlaps(float deltaTimeInMS);

	//temp function for slope colliders
	void CheckForSlopes(float deltaTimeInMS);

	//deltaTime = in seconds
	void UpdatePositions(float deltaTime);

	//obtains a new rate of speed to add to acceleration
	float GetDeltaSpeed(Kinematic* kinematic);

private:
	//do not need to free these pointers as these get freed in ecs
	ECS* ecs;
	ComponentManager<Transform>* transforms;
	ComponentManager<Kinematic>* kinematics;
	KeyboardController* keyboard;
};

#endif

