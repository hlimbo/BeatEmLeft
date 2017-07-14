#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "ComponentManager.h"
#include <vector>
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
	void UpdateKinematics(float deltaTime);
	
	//temp function
	void CheckForCollisions(float deltaTime, std::vector<Vect2>* tileCoordinates);

	void UpdatePositions(float deltaTime);

private:
	//do not need to free these pointers as these get freed in ecs
	ECS* ecs;
	ComponentManager<Transform>* transforms;
	ComponentManager<Kinematic>* kinematics;
	KeyboardController* keyboard;
};

#endif

