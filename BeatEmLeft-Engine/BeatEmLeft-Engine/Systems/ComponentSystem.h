#ifndef COMPONENT_SYSTEM_H
#define COMPONENT_SYSTEM_H

#include <string>
#include <unordered_map>
using namespace std;

class Component;

//There will be multiple component_systems in ECS where key = component type value = component system
//Component System will
//1. manage components of a single type
//2. register components by assigning a specific component an ID obtained from an entity
class ComponentSystem
{
public:
	ComponentSystem() {}
	~ComponentSystem();

	void AddComponent(int id, Component* component);

	//this level of granularity maybe needed if I plan to re-register
	//components to different entity ids...

	//returns the component to properly delete to prevent resource leak
	//throws an exception if id is invalid
	Component* RemoveComponent(int id);
	bool DeleteComponent(Component* component);

private:
	//key = component type, value = map<key = id,value = component>
	//unordered_map<string, unordered_map<int, Component*>> components;
	
	//another option is to create an array of componentsystems in ECS to hold labeled by type
	//key = entity id, value = Component* (which can be any kind of component derived from the Component class)
	unordered_map<int,Component*> components;

	//component type vs entity type 
	//entity type does not need to depend on component type
	//these two things are independent of each other
};

#endif

