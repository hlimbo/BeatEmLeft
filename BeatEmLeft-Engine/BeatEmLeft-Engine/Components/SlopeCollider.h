#ifndef SLOPE_COLLIDER_H
#define SLOPE_COLLIDER_H
#include <Vect2.h>
#include <typeinfo>

struct SlopeCollider
{
	SlopeCollider(Vect2 position)
	{
		this->position = position;
		type = typeid(SlopeCollider).name();
	}
	~SlopeCollider(){}

	float GetSlope()
	{
		//or simply return -height / width;
		return (position.y - (position.y + height)) / ((position.x + width) - position.x);
	}

	float width;
	float height;
	Vect2 position;
	const char* type;
};

#endif
