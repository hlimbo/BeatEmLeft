#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H
#include <Vect2.h>
#include <typeinfo>

struct BoxCollider
{
	BoxCollider() { type = typeid(BoxCollider).name(); }
	~BoxCollider() {}

	//position over the top-left corner of the box
	Vect2 position;
	float height;
	float width;
	const char* type;

	Vect2 center()
	{
		return Vect2(position.x + width / 2, position.y / 2);
	}

	float halfWidth()
	{
		return width / 2;
	}

	float halfHeight()
	{
		return height / 2;
	}

};

#endif
