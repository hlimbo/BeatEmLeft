#ifndef CIRCLE_COLLIDER_H
#define CIRCLE_COLLIDER_H
#include <Vect2.h>
#include <typeinfo>

struct CircleCollider
{
	//typeid(class or some other type).name(); isn't nix compliant
	//but works for msvc?
	CircleCollider() { type = typeid(CircleCollider).name();  }
	~CircleCollider() {}

	Vect2 position;
	float radius;
	const char* type;
};

#endif