//Entity-Component System (ECS)
/*
	Contains:
		1. a single EntitySystem
		2. multiple ComponentManagers

	EntitySystem and ComponentManagers are combined to centralize Initialization and Update logic
	of components. EntitySystem is part of this system because components 
	need to associate themselves with an entity's id. This system is an alternative
	to the object-oriented/inheritance approach of creating a game engine.

	Advantages:
		1. Code will be more maintainable 
		(the larger this project grows, the more apparent this benefit will be a plus)

	Disadvantages(Maybe):
		1. Hard to prototype game mechanics since you have to go through multiple systems
		to see something change...(it takes a lot of effort to rapidly prototype using this
		structure of code).
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
	ComponentManager* GetComponentManager(std::string componentType);
	void DeleteComponentManager(std::string ComponentType);

	int CreateEntity(std::string entityType);
	int RemoveEntity(std::string entityType, int id);

	int EntityCount(std::string entityType);

	std::vector<std::string> GetEntityTypes();
	std::vector<std::string> GetComponentTypes();

	//subject to deprecation (not to be in use anymore).
	std::vector<int> GetEntityIDs();
	std::vector<int> GetEntityIDs(std::string entityType);

private:
	EntitySystem* entitySystem;
	//key = component type,value = Component Manager that holds an internal map of key = id, value = component pairs
	std::unordered_map<std::string, ComponentManager*> componentManagers;
	
	std::vector<std::string> entityTypes;
	std::vector<std::string> componentTypes;	
};

#endif

