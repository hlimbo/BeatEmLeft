#ifndef ENTITY_SYSTEM_H
#define ENTITY_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <functional>
using namespace std;

//ECS - Entity Component System

//This is responsible for holding metadata (data about data)
//to associate itself with various components that can be "attached"
//to this "object".
// Each Entity will have a unique id based on its string type.
// ID Generation will handle how ids get assigned to each entity.
struct Entity
{
	string type;//programmer can set the "type" of this entity object
	int id = -1;//id will be set by id generation system
	string name;//type-id e.g. door0
};


//Entity System will manage
//1. creation of entities through unique id generation based on type ~ done
//2. deletion of entities by id ~ done
//		a. will also delete components "attached" to this entity ? ~ I will need the component part of this to test
//		a. can probably be handled in the component system part since the EntitySystem can return the ids
//		a. of the type that was removed.

class EntitySystem
{
public:
	EntitySystem() {}
	~EntitySystem(){}

	bool ContainsEntityType(const string& type)
	{
		return entitiesByType.find(type) != entitiesByType.end();
	}

	//bug that can be introduced is if key mappings cannot be removed...
	//then an entity mapping of vector size 0 will return true even though
	//it was already inserted..

	//returns true if a NEW entity type is added into the system
	//returns false if entity to be inserted already exists in the system
	bool AddEntityType(const string& type)
	{
		return entitiesByType[type].empty();
	}

	//returns the id of the entity after creation of entity
	//otherwise return -1 indicating that the entity has already been 
	//created as a new copy and placed in the system.
	int CreateEntity(Entity& entity)
	{

		if (entity.id != -1)
			return -1;
		
		//id generation system
		int newID;
		if (AddEntityType(entity.type))
		{
			newID = 0;
		}
		else
		{
			//reuse ids that were removed from the system
			if (!unusedIDs[entity.type].empty())
			{
				newID = unusedIDs[entity.type].top();
				unusedIDs[entity.type].pop();
			}
			else
			{
				newID = entitiesByType[entity.type].size();
			}
		}

		entity.id = newID;
		entity.name = entity.type + to_string(newID);
		entitiesByType[entity.type].push_back(entity);

		return newID;
	}

	//returns the id the entity to be removed on success
	//returns -1 on failure
	int RemoveEntity(string type, int id)
	{
		if (!ContainsEntityType(type))
			return -1;
		else
		{
			auto it = entitiesByType[type].begin();
			for (;it != entitiesByType[type].end();++it)
			{
				if (it->id == id)
					break;
			}

			if (it != entitiesByType[type].end())
			{
				int removedID = it->id;
				swap(*it, entitiesByType[type].back());
				entitiesByType[type].pop_back();

				//delete the entity type from the system 
				//could be bad for performance if constantly creating and deleting
				//the same entity types
				if (entitiesByType[type].empty())
					entitiesByType.erase(type);

				//whenever an entity is removed, removedIDs should be recycled when
				//creating new Entities of the same type
				unusedIDs[type].push(removedID);

				return removedID;
			}
	
		}

		return -1;
	}

	int EntityCount(string type)
	{
		return entitiesByType.at(type).size();
	}

	//this returns a pointer to the internal unorder_map datamember where
	//its members cannot be modified
	const vector<Entity>* GetEntities(string type)
	{
		return &entitiesByType.at(type);
	}

	//returns a separate copy of the entity object from the internal map
	Entity GetEntity(string type, int index)
	{
		return entitiesByType.at(type).at(index);
	}

	//returns a pointer to the entity object contained in this service
	//where the entity's variables cannot be modified due to the const keyword
	//const Entity* GetEntity(string type, int id)
	//{
	//	return entitiesByType.at(type)[id];
	//}

	int TypesCount()
	{
		return entitiesByType.size();
	}

	//This function not only removes the Entity Type from the system,
	//it also removes all entities with the matching type from the system
	//use this function carefully ~ could rename to RemoveAllEntities(string type)
	bool RemoveEntityType(string type)
	{
		auto it = entitiesByType.find(type);
		if (it != entitiesByType.end())
		{
			entitiesByType.erase(type);
			unusedIDs.erase(type);
			return true;
		}

		return false;
	}

private:
	unordered_map<string, vector<Entity>> entitiesByType;
	//key = entity type
	//value is a priority queue that holds a list of ids that have been removed
	//from the entity system sorted by lowest integer value going first.
	unordered_map < string, priority_queue<int, vector<int>, greater<int>>> unusedIDs;

};


#endif
