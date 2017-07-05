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

	virtual void Init() = 0;
	virtual void Update(float deltaTime) = 0;
	
	std::string GetType() { return componentType;  }
protected:
	std::string componentType;
};


#endif
