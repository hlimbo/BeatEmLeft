#ifndef KINEMATIC_COMPONENT_H
#define KINEMATIC_COMPONENT_H
#include <Vect2.h>
#include <typeinfo.h>
/*
	Defines 2d physics variables used to move entities around the game world
	e.g. velocity and acceleration
*/

struct Kinematic
{
	Kinematic() { type = typeid(Kinematic).name(); }
	~Kinematic() {}

	Vect2 velocity; // pixels / second with direction applied
	Vect2 acceleration; // pixels / second / second
	Vect2 direction; //serves as a 2d unit vector (see unit circle to visualize direction)	
	
	//minSpeed >= 0 and maxSpeed >= 0
	float minSpeed; // pixels / second
	float maxSpeed; // pixels / second
	float currentSpeed; // pixels / second
	float gravity;// pixels / second / second

	//float mass?

	const char* type;
};

#endif