#include "RenderSystem.h"
#include "ECS.h"
#include "ComponentManager.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL2/SDL_render.h>
#include <vector>

using namespace std;

//**1. do I pass in ECS* to every system? ~ keep ecs and system separate!
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

RenderSystem::RenderSystem()
{
	transformManager = spriteManager = nullptr;
}

RenderSystem::RenderSystem(ECS* ecs)
{
	this->ecs = ecs;
	transformManager = ecs->GetComponentManager("Transform");
	spriteManager = ecs->GetComponentManager("Sprite");
}

RenderSystem::~RenderSystem()
{
}

//call order ~ not sure yet on the order
/*
	PlayerControl::Update(deltaTime);
	MovementSystem::Update(deltaTime);
	CollisionSystem::Update(deltaTime);
	RenderSystem::Update(deltaTime);
	ECS::Update(deltaTime);~ updates components that don't need other components to update itself.
	RenderSystem::Draw(render);
*/

//problem: ecs::update(); updates the controller and keyboard states within the component
//and will also rendercopy images in each sprite component?
//question: should the respective systems handle states changing or should 
//the components themselves handle those changes internally?

//if systems handle all the updating.. this would get rid of..
//1. component update() function and possibly init() function, making components purely based on data
//2. ecs::update() will not need update anymore since components do not update themselves

//I could pass SDL_Renderer* here instead and would not need to include
//a SDL_Renderer* pointer as a data member...
void RenderSystem::Update(float deltaTime, SDL_Renderer* render)
{
	vector<int> ids = ecs->GetEntityIDs();
	for (auto it = ids.begin();it != ids.end();++it)
	{
		auto transform = transformManager->GetComponent<TransformComponent>(*it);
		auto sprite = spriteManager->GetComponent<SpriteComponent>(*it);

		if (transform != nullptr && sprite != nullptr)
		{
			//convert floating point coordinates to screen integer coordinates
			float diffx = transform->position.x - floorf(transform->position.x);
			float diffy = transform->position.y - floorf(transform->position.y);

			sprite->x = (diffx > 0.5f) ? (int)ceilf(transform->position.x) : (int)floorf(transform->position.x);
			sprite->y = (diffy > 0.5f) ? (int)ceilf(transform->position.y) : (int)floorf(transform->position.y);
			sprite->Draw(render);
		}
	}
}

void RenderSystem::Draw(SDL_Renderer* render)
{
	SDL_RenderPresent(render);
	SDL_RenderClear(render);
}

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
