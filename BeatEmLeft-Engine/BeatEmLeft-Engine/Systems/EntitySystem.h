#ifndef ENTITY_SYSTEM_H
#define ENTITY_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
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
//1. creation of entities through unique id generation based on type **
//2. deletion of entities by id
//		a. will also delete components "attached" to this entity

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
		//TODO: if an entity was removed, make sure to reuse
		//the id of the entity that was just removed..
		//store ids that were removed from the system into a priority queue
		//where the lowest available id for a type is available

		//this will work similar to how fds get reused in C

		if (entity.id != -1)
			return -1;

		//generate a unique id based on its type
		int newID;
		if (AddEntityType(entity.type))
		{
			newID = 0;
		}
		else
		{
			newID = entitiesByType[entity.type].size();
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
				return removedID;
			}
	
		}

		return -1;
	}

	int EntityCount(string type)
	{
		return entitiesByType.at(type).size();
	}

	vector<Entity>* GetEntities(string type)
	{
		return &entitiesByType.at(type);
	}

	int TypesCount()
	{
		return entitiesByType.size();
	}

	bool RemoveEntityType(string type)
	{
		auto it = entitiesByType.find(type);
		if (it != entitiesByType.end())
		{
			int val = entitiesByType.erase(type);
			return true;
		}

		return false;
	}

private:
	unordered_map<string, vector<Entity>> entitiesByType;

};


#endif
