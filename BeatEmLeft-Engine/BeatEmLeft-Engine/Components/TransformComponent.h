#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H	
#include "../Systems/Component.h"
#include <Vect2.h>

//use this component to modify the game world coordinates and properties
//of the entity that registered to this component
class TransformComponent :
	public Component
{
public:
	TransformComponent(std::string type) : Component(type) {}
	virtual ~TransformComponent() {}

	virtual void Init();
	virtual void Update(float deltaTime);

	Vect2 position;
	Vect2 scale;
};

#endif

