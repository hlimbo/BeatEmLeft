#include "ECS.h"
#include "EntitySystem.h"
#include "ComponentSystem.h"
#include "Component.h"

using namespace std;

ECS::ECS()
{
	entitySystem = new EntitySystem();
}

ECS::~ECS()
{
	delete entitySystem;

	for (auto it = componentSystems.begin();it != componentSystems.end(); /* Empty */)
	{
		//might not need since testing is only happening when allocating components to the stack..
		ComponentSystem* componentSystem = it->second;
		it = componentSystems.erase(it);
		delete componentSystem;
	}
}

void ECS::Init()
{
	vector<int> entityIDs = GetEntityIDs();

	for (auto it = componentSystems.begin();it != componentSystems.end();++it)
	{
		ComponentSystem* system = it->second;
		for (int id : entityIDs)
		{
			system->GetComponent(id)->Init();
		}
	}
}

void ECS::Update(float deltaTime)
{
	vector<int> entityIDs = GetEntityIDs();

	//component updates
	for (auto it = componentSystems.begin();it != componentSystems.end();++it)
	{
		ComponentSystem* system = it->second;
		for (int id : entityIDs)
		{
			system->GetComponent(id)->Update(deltaTime);
		}
	}
}


//return true if a new entity type is added into the system..
//return false otherwise
bool ECS::AddEntityType(std::string newType)
{
	//adds the entity type into the system if it does not exist, otherwise it does not add it in and returns false
	if (!entitySystem->AddEntityType(newType))
		return false;

	entityTypes.push_back(newType);//used for book keeping all entity types available in the system
	return true;
}

bool ECS::AddComponentType(std::string newType)
{
	auto it = find(componentTypes.begin(), componentTypes.end(), newType);
	if (it != componentTypes.end())
	{
		return false;
	}

	componentTypes.push_back(newType);
	return true;
}

bool ECS::RegisterComponentToEntity(std::string componentType, int id, Component* component)
{	
	return componentSystems[componentType]->AddComponent(id, component);
}

Component* ECS::UnregisterComponentFromEntity(std::string componentType, int id)
{
	return componentSystems.at(componentType)->RemoveComponent(id);
}

bool ECS::CreateComponentSystem(ComponentSystem* componentSystem)
{
	string type = componentSystem->GetType();
	if (componentSystems[type] == nullptr)
	{
		componentSystems[type] = componentSystem;
		componentTypes.push_back(type);
		return true;
	}

	return false;
}

//type = componentSystem container type
//e.g. type can be a componentSystem that only contains renderComponents
//does not delete a componentSystem that is not already in the map
void ECS::DeleteComponentSystem(string type)
{
	ComponentSystem* system = componentSystems[type];
	componentSystems.erase(type);
	if (system != nullptr)
	{
		delete system;
		system = nullptr;
	}
}

int ECS::CreateEntity(std::string entityType)
{
	AddEntityType(entityType);
	return entitySystem->CreateEntity(entityType);
}

int ECS::RemoveEntity(std::string entityType, int id)
{
	int idt = entitySystem->RemoveEntity(entityType, id);
	if (idt != -1)
	{
		if (!entitySystem->ContainsEntityType(entityType))
		{
			auto it = find(entityTypes.begin(), entityTypes.end(), entityType);
			swap(*it, entityTypes.back());
			entityTypes.pop_back();
		}

		for (string componentType : componentTypes)
		{
			UnregisterComponentFromEntity(componentType, idt);
		}
	}

	return idt;
}

int ECS::EntityCount(std::string entityType)
{
	return entitySystem->EntityCount(entityType);
}

vector<string> ECS::GetEntityTypes()
{
	return entityTypes;
}
vector<string> ECS::GetComponentTypes()
{
	return componentTypes;
}

vector<int> ECS::GetEntityIDs()
{
	vector<int> entityIDs;
	//can simplify down to one loop (just use a vector<Entity>)
	//can probably have this code in init if I'm not removing
	//any entities from the system?

	//probably the benefit of this design is to make sure only certain
	//entities can receive regular updates while other entity types 
	//marked with the disabled flag don't update?
	for (string entityType : entityTypes)
	{
		auto it = entitySystem->GetEntities(entityType)->begin();
		auto end = entitySystem->GetEntities(entityType)->end();
		for (;it != end;++it)
		{
			entityIDs.push_back(it->id);
		}
	}

	return entityIDs;
}




