#include "ECS.h"
#include "EntitySystem.h"
#include "ComponentManager.h"
#include "Component.h"

using namespace std;

ECS::ECS()
{
	entitySystem = new EntitySystem();
}

ECS::~ECS()
{
	delete entitySystem;

	for (auto it = componentManagers.begin();it != componentManagers.end(); /* Empty */)
	{
		//might not need since testing is only happening when allocating components to the stack..
		ComponentManager* componentManager = it->second;
		it = componentManagers.erase(it);
		delete componentManager;
	}
}

//this is possibly a bad model!
void ECS::Init()
{
	//should be safe to call as long as the entity system add/remove entities before this function call.
	vector<int> entityIDs = entitySystem->GetIDs();

	for (auto it = componentManagers.begin();it != componentManagers.end();++it)
	{
		ComponentManager* manager = it->second;
		for (auto id = entityIDs.begin();id != entityIDs.end();++id)
		{
			Component* component = manager->GetComponent(*id);
			if (component != nullptr)
				component->Init();

			//pseudo code
			/* if(component != nullptr)
					component->Init();
					SpriteComponent* spriteComp = manager->GetComponent<SpriteComponent>(*id);
					if(spriteComp != nullptr)
						spriteComp->SetTextureAttributes("image fileName to initialize")
			*/
		}
	}
}

void ECS::Update(float deltaTime)
{
	//should be safe to call as long as entities get removed/added before this update call gets invoked.
	vector<int> entityIDs = entitySystem->GetIDs();

	//component updates
	for (auto it = componentManagers.begin();it != componentManagers.end();++it)
	{
		ComponentManager* manager = it->second;
		for (auto id = entityIDs.begin();id != entityIDs.end();++id)
		{
			Component* component = manager->GetComponent(*id);
			if (component != nullptr)
				component->Update(deltaTime);
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

//initialize all components from heap
//returns true if component can be registed to entity
//returns false if component of same type was already registered to entity id
//or if component doesn't doesn't match the component type supported
bool ECS::RegisterComponentToEntity(int id, Component* component)
{	
	string componentType = component->GetType();
	return componentManagers[componentType]->AddComponent(id, component);
}

Component* ECS::UnregisterComponentFromEntity(std::string componentType, int id)
{
	return componentManagers.at(componentType)->RemoveComponent(id);
}

bool ECS::CreateComponentManager(ComponentManager* componentSystem)
{
	string type = componentSystem->GetType();
	if (componentManagers[type] == nullptr)
	{
		componentManagers[type] = componentSystem;
		componentTypes.push_back(type);
		return true;
	}

	return false;
}

ComponentManager* ECS::GetComponentManager(std::string componentType)
{
	if (componentManagers[componentType] == nullptr)
	{
		componentManagers.erase(componentType);
		return nullptr;
	}

	return componentManagers[componentType];
}

//type = componentManager container type
//e.g. type can be a componentManager that only contains renderComponents
//does not delete a componentManager that is not already in the map
void ECS::DeleteComponentManager(string type)
{
	ComponentManager* manager = componentManagers[type];
	componentManagers.erase(type);

	//remove componentType from the manager
	auto it = find(componentTypes.begin(), componentTypes.end(), type);
	if (it != componentTypes.end())
	{
		swap(*it, componentTypes.back());
		componentTypes.pop_back();
	}

	if (system != nullptr)
	{
		delete manager;
		manager = nullptr;
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

		for (auto it = componentTypes.begin();it != componentTypes.end();++it)
		{
			Component* component = UnregisterComponentFromEntity(*it, idt);
			delete component;
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
	return entitySystem->GetIDs();
}

std::vector<int> ECS::GetEntityIDs(std::string entityType)
{
	return entitySystem->GetIDs(entityType);
}




