#include "ComponentSystem.h"
#include "Component.h"

ComponentSystem::~ComponentSystem()
{
	//all components registered into this system will be deleted when component-system leaves its scope
	for (auto it = components.begin();it != components.end(); ++it)
	{
		Component* component = it->second;
		components.erase(it);
		delete component;
	}
}

void ComponentSystem::AddComponent(int id, Component* component)
{
	//components.insert(pair<int, Component*>(id, component));
	//components[id] = component;
	components.insert(make_pair(id, component));
}

Component* ComponentSystem::RemoveComponent(int id)
{
	//Note:: at() throws an exception if the id is invalid
	Component* component = components.at(id);
	components.erase(id);
	return component;
}

bool ComponentSystem::DeleteComponent(Component* component)
{
	if (component == nullptr)
		return false;

	delete component;
	return true;
}