#include "LevelFactory.h"
#include "../Systems/ECS.h"

#include <iostream>
#include <fstream>

LevelFactory::LevelFactory(ECS* ecs, std::string basepath)
{
	this->ecs = ecs;
	this->basepath = basepath;
}
LevelFactory::LevelFactory()
{
	ecs = nullptr;
}

LevelFactory::~LevelFactory()
{
}

bool LevelFactory::ReadJsonFile(std::string filename)
{
	std::string filepath = basepath + filename;
	ifstream file(filepath);
	if (!file.good())
	{
		cout << "Could not open: " << filepath << endl;
		return false;
	}

	reader.parse(file, root, false);
	file.close();
	return true;
}

void LevelFactory::CreateComponents()
{
	for (auto it = root.begin();it != root.end();++it)
	{
		int id = ecs->entitySystem.CreateEntity(it.name());
		Json::Value components = root[it.name()]["components"];
		for (unsigned int i = 0;i < components.size();++i)
		{
			std::string componentName = components[i].asString();

			//there's probably a better way to do this..
			if (componentName == "Transform")
			{
				ecs->transforms.AddComponent(id, new Transform());
			}
			else if (componentName == "Sprite")
			{
				ecs->sprites.AddComponent(id, new Sprite());
			}
			else if (componentName == "BoxCollider")
			{
				ecs->boxColliders.AddComponent(id, new BoxCollider());
			}
		}

	}
}

void LevelFactory::InitComponents()
{
	std::vector<int> ids = ecs->entitySystem.GetIDs();

	for (auto it = ids.begin();it != ids.end();++it)
	{
		Transform* transform = ecs->transforms.GetComponent(*it);
		Sprite* sprite = ecs->sprites.GetComponent(*it);
		BoxCollider* box = ecs->boxColliders.GetComponent(*it);

		if (transform != nullptr)
		{

		}

		if (sprite != nullptr)
		{

		}

		if (box != nullptr)
		{

		}
	}
}
