#ifndef KINEMATIC_COMPONENT_H
#define KINEMATIC_COMPONENT_H
#include <Vect2.h>
#include <typeinfo>
/*
	Defines 2d physics variables used to move entities around the game world
	e.g. velocity and acceleration
*/

struct Kinematic
{
	Kinematic() : 
			type(typeid(Kinematic).name()), 
			accelFactor(0.5f),
			minSpeed(0),
			maxSpeed(0),
			currentSpeed(0),
			gravity(0),
			minGravity(0),
			maxGravity(0),
			gravityFactor(0),
			minJumpSpeed(0),
			maxJumpSpeed(0),
			jumpFactor(0),
			jumpSpeed(0),
			maxJumpTime(0),
			currentJumpTime(0),
			mass(0) {}
	~Kinematic() {}

	Vect2 velocity; // pixels / millisecond with direction applied
	Vect2 acceleration; // pixels / millisecond / millisecond
	Vect2 direction; //serves as a 2d unit vector (see unit circle to visualize direction)	
	
	//minSpeed >= 0 and maxSpeed >= 0
	float minSpeed; // pixels / millisecond
	float maxSpeed; // pixels / millisecond
	float currentSpeed; // pixels / millisecond
	//Acceleration factor = change this value to modify the rate at which the speed changes.
	//by default, all Kinematic objects have an acceleration factor of 0.5f
	float accelFactor; // pixels / millisecond / millisecond

	float gravity;// pixels / millisecond / millisecond
	float minGravity;
	float maxGravity;
	float gravityFactor;

	//should make more components for these things possibly
	//jumping
	float minJumpSpeed;
	float maxJumpSpeed;
	float jumpFactor;
	float jumpSpeed;
	float maxJumpTime;
	float currentJumpTime;

	float mass;

	const char* type;
};

#endif