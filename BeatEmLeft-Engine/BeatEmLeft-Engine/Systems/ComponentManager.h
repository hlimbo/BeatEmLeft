#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include <unordered_map>
#include <typeinfo.h>

//Component Manager will
//1. manage components of a single type
//2. register components by assigning a specific component an ID obtained from an entity
//3. remove components based on entity id


//Component Manager holds a map of components where each component in the map is registered to a valid entity id.
//This templated class serves as a container for components of a single type where the logic will be handled in
//some systems class.
//This templated class ensures no inheritance is required when creating new component classes.

template<typename component>
class ComponentManager
{
public:
	explicit ComponentManager() { type = typeid(component).name(); }
	~ComponentManager()
	{
		int destroyCount = 0;
		//all components pointers registered to the system will be removed once it leaves scope..
		for (auto it = components.begin();it != components.end(); /* Empty */)
		{
			//might not need since testing is only happening when allocating components to the stack..
			component* component = it->second;
			//components.erase(it) returns the next iterator after the erased one.
			it = components.erase(it);
			delete component;
			
			++destroyCount;
		}

		//debug
		printf("Successfully destroyed %d %s components\n", destroyCount, type);
	}

	bool AddComponent(int id, component* component)
	{
		//here don't allow an entity to add the component of the same type
		//more than once
		//this is possibly an O(n) check...
		if (components.find(id) != components.end())
			return false;

		//I don't think I will need this since this class is a templated class.
		//here don't allow to add a component of a different type
		//if (type != "" &&  strcmp(component->type,type) != 0)
			//return false;

		components.insert(make_pair(id, component));
		return true;
	}

	component* GetComponent(int id)
	{
		if (components[id] == nullptr)
		{
			RemoveComponent(id);
			return nullptr;
		}

		return components.at(id);
	}

	//current design flow: only 1 entity id can be assigned per component type
	//e.g. an entity cannot have more than 1 render component

	//returns the component to properly delete to prevent resource leak
	//throws an exception if id is invalid
	//when a component is removed... programmer is responsible for deleting it
	component* RemoveComponent(int id)
	{
		component* component = components[id];
		components.erase(id);
		return component;
	}

	//programmer is not responsible for deleting the component afterwards since
	//this function does the deletion for you to prevent memory leaks.
	//Deletes a component if id passed in is valid, otherwise this is treated as a no-op if
	//id is invalid.
	void DeleteComponent(int id)
	{
		component* component = components[id];
		components.erase(id);
		if (component != nullptr)
		{
			delete component;
			component = nullptr;
		}

	}

	int GetComponentCount() { return components.size();  }
	bool IsEmpty() { return components.empty();  }
	const char* type;

private:
	std::unordered_map<int,component*> components;

};

#endif

