#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H	
#include <Vect2.h>

//use this component to modify the game world coordinates and properties
//of the entity that registered to this component
struct Transform
{
	Transform() { type = typeid(Transform).name(); }
	Vect2 position;
	Vect2 scale;
	const char* type;
};

#endif

