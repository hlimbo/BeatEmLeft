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
class ComponentManager //could make this into a template class...but would have to put all code in header..
{
public:
	ComponentManager() {}
	ComponentManager(string type) { this->type = type; }
	~ComponentManager();

	bool AddComponent(int id, Component* component);
	Component* GetComponent(int id);

	//https://gamedev.stackexchange.com/questions/55950/how-can-i-properly-access-the-components-in-my-c-entity-component-systems
	template <typename T>
	T* GetComponent(int id)
	{
		if (components[id] == nullptr)
		{
			RemoveComponent(id);
			return nullptr;
		}

		//could use static_cast since it will be faster....

		//use dynamic_cast to do a check during runtime if the component is truly the supplied type
		//otherwise, it will return nullptr if the wrong typed object was passed.
		return dynamic_cast<T*>(components.at(id));
	}

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

};

#endif

