#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "ComponentManager.h"

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
	void UpdatePositions(float deltaTime);

private:
	//do not need to free these pointers as these get freed in ecs
	ECS* ecs;
	ComponentManager<Transform>* transforms;
	ComponentManager<Kinematic>* kinematics;
	KeyboardController* keyboard;
};

#endif

