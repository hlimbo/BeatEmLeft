#include "RenderSystem.h"
#include "ComponentManager.h"
#include "EntitySystem.h"


//1. do I pass in ECS* to every system?
//2. do I pass in only the componentManagers and entitySystem to
//every system I create as a class?

//3. do I include some sort of generic System class where every
//system can inherit from  in order to store all types of systems
//into ecs?

//in ecs class:
//CreateSystem(new System("RenderSystem"));
//DeleteSystem("RenderSystem");
//RegisterComponentManagerToSystem("TransformManager","RenderSystem");
//RegisterComponentManagerToSystem("SpriteManager", "RenderSystem");

//unordered_map<std::string,System*> systems;

//in systems class:
//unordered_map<std::string,ComponentManager*> managers;

//in systems::update(deltaTime)
/*
	vector<int> ids = entitySystem->GetIDs();

	ComponentManager* transformManager = managers.at("transformManager");
	ComponentManager* spriteManager = managers.at("spriteManager");

	for(auto it = ids.begin();it != ids.end();++it)
	{
		//there's probably a cleaner way of doing this.. not sure how I would achieve this..
		//e.g. transformManager->GetComponent<TransformComponent>(*it);
		
		TransformComponent* transform = 
		dynamic_cast<TransformComponent*>(transformManager->GetComponent(*it));
		
		SpriteComponent* sprite = 
		dynamic_cast<SpriteComponent*>(spriteManager->GetComponent(*it));
		
		if(transform != nullptr && sprite != nullptr)
		{
			//here we know that it belongs to the system
			//without the need of type checking since 
			//the manager already ensures all components added to it
			//are of a single type e.g. ComponentManager A only holds
			//TransformComponents

			//convert floating point coordinates to screen integer coordinates
			float diffX = transform->x - floor(transform->x);
			float diffY = transform->y - floor(transform->y);

			sprite->x = (diffX > 0.5) ? (int)ceil(transform->x) : (int)floor(transform->x);
			sprite->y = (diffY > 0.5) ? (int)ceil(transform->y) : (int)floor(transform->y);


			//might not need to update the individual components here...
			//only access its data here and change it if necessary and let the component managers
			//in ecs handle  the updates (drawing image to screen to proper coordinates).
			sprite->draw(//NEED RENDERER HERE//);
			..SDL_RenderCopy(//NEED RENDERER HERE//,sprite->texture?,NULL,&sprite->bounds?);

			//may not need to call since ecs calls every update function of every component
			//sprite->update(deltaTime);//sprite->update() will just call SDL_RenderCopy();
		}
	}

	//** (may do this one instead) One way to fix this
	//easily is to just pass in an SDL_Renderer* when the 
	//Render system is constructed..
	SDL_RenderPresent(//NEED RENDERER HERE//);
	SDL_RenderClear(//NEED RENDERER HERE//);
*/

RenderSystem::RenderSystem()
{
}


RenderSystem::~RenderSystem()
{
}

void RenderSystem::Update(float deltaTime)
{
}
