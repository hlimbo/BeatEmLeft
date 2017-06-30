#ifndef COMPONENT_H
#define COMPONENT_H

class Component
{
public:
	Component() {}
	virtual ~Component() {}

	void Init() {}
	void Update(float deltaTime) {}
};


#endif
