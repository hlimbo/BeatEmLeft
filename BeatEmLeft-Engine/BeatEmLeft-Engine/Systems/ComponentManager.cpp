#include "ComponentManager.h"
#include "Component.h"

//removes all component pointers from the system
//I plan to create all components in the heap
//via function call AddComponent(0,new RenderComponent("render"));
ComponentManager::~ComponentManager()
{
	//all components pointers registered to the system will be removed once it leaves scope..
	for (auto it = components.begin();it != components.end(); /* Empty */)
	{
		//might not need since testing is only happening when allocating components to the stack..
		Component* component = it->second;
		//components.erase(it) returns the next iterator after the erased one.
		it = components.erase(it);
		delete component;
	
	}
}

//possibly rename to RegisterComponent
//returns true if component is added into the system
//false otherwise
bool ComponentManager::AddComponent(int id, Component* component)
{
	//here don't allow an entity to add the component of the same type
	//more than once
	//this is possibly an O(n) check...
	if(components.find(id) != components.end())
		return false;

	//here don't allow to add a component of a different type
	if (type != "" && component->GetType() != type)
		return false;
	
	components.insert(make_pair(id, component));
	return true;
}
Component* ComponentManager::GetComponent(int id)
{
	if (components[id] == nullptr)
	{
		RemoveComponent(id);
		return nullptr;
	}

	return components.at(id);
}

Component* ComponentManager::RemoveComponent(int id)
{
	Component* component = components[id];
	components.erase(id);
	return component;
}
