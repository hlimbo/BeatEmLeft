#include "ECS.h"
#include "EntitySystem.h"
#include "ComponentSystem.h"

ECS::ECS()
{
	entitySystem = new EntitySystem();
	componentSystem = new ComponentSystem();
}

ECS::~ECS()
{
	delete entitySystem;
	delete componentSystem;
}