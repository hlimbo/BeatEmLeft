#ifndef COMPONENT_H
#define COMPONENT_H
#include <string>

class Component
{
public:
	Component(std::string type) 
	{
		componentType = type;
	}
	virtual ~Component() {}

	virtual void Init() {}
	virtual void Update(float deltaTime) {}
	
	std::string GetType() { return componentType;  }
protected:
	std::string componentType;
};


#endif
