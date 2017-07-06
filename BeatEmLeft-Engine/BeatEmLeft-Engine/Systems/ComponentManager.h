#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <string>
#include <unordered_map>
using namespace std;

class Component;

//There will be multiple component_managers in ECS where key = component type value = component manager
//Component Manager will
//1. manage components of a single type
//2. register components by assigning a specific component an ID obtained from an entity
//3. remove components based on entity id
class ComponentManager
{
public:
	ComponentManager() {}
	ComponentManager(string type) { this->type = type; }
	~ComponentManager();

	bool AddComponent(int id, Component* component);
	Component* GetComponent(int id);

	//Currently, there is not component type checking before we can
	//add the component to the system.. componentSystem can hold
	//a mixed bag of components...
	//Design choice: don't want to hold a mixed bag of components
	//in the componentSystem because each component is mapped to
	//a single entity id which can be reused exactly once per
	//component system.

	//current design flow: only 1 entity id can be assigned per component type
	//e.g. an entity cannot have more than 1 render component

	//returns the component to properly delete to prevent resource leak
	//throws an exception if id is invalid
	Component* RemoveComponent(int id);

	int GetComponentCount() { return components.size();  }
	bool IsEmpty() { return components.empty();  }
	string GetType() { return type; }

	//TODO: Create virtual Init and Update() functions ~ this will serve as a base class..
	//virtual void Init();
	//virtual void Update(float deltaTime);

private:
	//key = component type, value = map<key = id,value = component>
	//unordered_map<string, unordered_map<int, Component*>> components;
	
	//another option is to create an array of componentsystems in ECS to hold labeled by type
	//key = entity id, value = Component* (which can be any kind of component derived from the Component class)
	unordered_map<int,Component*> components;
	string type;

	//component type vs entity type 
	//entity type does not need to depend on component type
	//these two things are independent of each other
};

#endif

