//Entity-Component System (ECS)
/*
	Contains:
		1. a single EntitySystem
		2. multiple ComponentManagers

	EntitySystem and ComponentManagers are combined to centralize Initialization and Update logic
	of components. EntitySystem is part of this system because components 
	need to associate themselves with an entity's id. This system is an alternative
	to the object-oriented/inheritance approach of creating a game engine.
*/
#ifndef ECS_H
#define ECS_H

#include <unordered_map>
#include <vector>
#include <string>

class EntitySystem;
class ComponentManager;
class Component;

class ECS
{
public:
	ECS();
	~ECS();

	void Init();
	void Update(float deltaTime);
	
	bool AddEntityType(std::string newType);
	bool AddComponentType(std::string newType);

	bool RegisterComponentToEntity(int id, Component* component);
	Component* UnregisterComponentFromEntity(std::string componentType, int id);

	bool CreateComponentManager(ComponentManager* componentSystem);
	void DeleteComponentManager(std::string ComponentType);

	int CreateEntity(std::string entityType);
	int RemoveEntity(std::string entityType, int id);

	int EntityCount(std::string entityType);

	std::vector<std::string> GetEntityTypes();
	std::vector<std::string> GetComponentTypes();

	//subject to deprecation (not to be in use anymore).
	std::vector<int> GetEntityIDs();

private:
	EntitySystem* entitySystem;
	//key = component type,value = Component Manager that holds an internal map of key = id, value = component pairs
	std::unordered_map<std::string, ComponentManager*> componentManagers;
	
	std::vector<std::string> entityTypes;
	std::vector<std::string> componentTypes;	
};

#endif

