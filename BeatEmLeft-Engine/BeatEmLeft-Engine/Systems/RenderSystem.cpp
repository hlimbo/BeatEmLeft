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
	ecs = nullptr;
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

void RenderSystem::Init(int cameraWidth,int cameraHeight)
{
	camera.x = 0;
	camera.y = 0;
	camera.w = cameraWidth;
	camera.h = cameraHeight;
	//this should be the playerID
	int playerID = ecs->GetEntityIDs("Player").at(0);
	SetEntityToFollow(playerID);
}

void RenderSystem::SetLocation(int x, int y)
{
	camera.x = x;
	camera.y = y;
}

//temporary level dimensions
#define LEVEL_WIDTH 1024
#define LEVEL_HEIGHT 920
//#define LEVEL_WIDTH 2048
//#define LEVEL_HEIGHT 1840
bool RenderSystem::SetEntityToFollow(int id)
{
	//target entity's components
	TransformComponent* transformComponent = transformManager->GetComponent<TransformComponent>(id);
	SpriteComponent* spriteComponent = spriteManager->GetComponent<SpriteComponent>(id);
	if (transformComponent != nullptr && spriteComponent != nullptr)
	{
		SDL_Point targetPos = getFloatToIntegerCoordinates(transformComponent->position);
		
		//set a camera offset relative to the entity ~ camera centered on target entity
		camera.x = (targetPos.x + spriteComponent->width / 2) - (camera.w / 2);
		camera.y = (targetPos.y + spriteComponent->height / 2) - (camera.h / 2);

		//camera bounds check relative to world coordinates
		if (camera.x < 0)
			camera.x = 0;
		if (camera.y < 0)
			camera.y = 0;
		if (camera.x + camera.w > LEVEL_WIDTH)
			camera.x = LEVEL_WIDTH - camera.w;
		if (camera.y + camera.h > LEVEL_HEIGHT)
			camera.y = LEVEL_HEIGHT - camera.h;
	}

	return transformComponent != nullptr && spriteComponent != nullptr;
}

//I could pass SDL_Renderer* here instead and would not need to include
//a SDL_Renderer* pointer as a data member...
void RenderSystem::Update(float deltaTime, SDL_Renderer* render)
{

	//camera tracking
	int backgroundID = ecs->GetEntityIDs("Background").at(0);

	int playerID = ecs->GetEntityIDs("Player").at(0);
	SetEntityToFollow(playerID);

	vector<int> ids = ecs->GetEntityIDs();
	for (auto it = ids.begin();it != ids.end();++it)
	{
		auto transform = transformManager->GetComponent<TransformComponent>(*it);
		auto sprite = spriteManager->GetComponent<SpriteComponent>(*it);

		if (transform != nullptr && sprite != nullptr)
		{
			//convert floating point coordinates to screen integer coordinates
			//using the round function (rounds values up if >= 0.5 otherwise round down)
			sprite->x = (int)roundf(transform->position.x);
			sprite->y = (int)roundf(transform->position.y);
			
			//benefit of this approach is that I have more control over how I want to round my floating coordinates to
			//negative of this approach: I have to write a little bit more code to do this...
			//SDL_Point newPoints = getFloatToIntegerCoordinates(transform->position);

			//temp code ~ needs to be refactored...
			if (backgroundID == *it)
			{
				//always render the background since all of its parts will always be seen in the game.
				SDL_RenderCopy(render, sprite->texture, &camera, NULL);
			}
			else if (playerID == *it)
			{
				//this effect makes it as if the camera is following the player.
				//but in reality this is making sure the player stays withing bounds of the camera.
				//convert player world coordinates to screen coordinates relative to camera
				SDL_Rect screenBounds;
				screenBounds.x = sprite->x - camera.x;
				screenBounds.y = sprite->y - camera.y;
				screenBounds.w = sprite->width;
				screenBounds.h = sprite->height;
				SDL_RenderCopy(render, sprite->texture, NULL/* source texture is used if I want to animate the sprite*/, &screenBounds);
			}
			else //for all other sprites... render them  (right now render order happens from lowest entity id to highest entity id)
			{
				//render the sprite if it is seen by the camera (check if sprite is within screen bounds)
				SDL_Rect screenCoords;
				screenCoords.x = sprite->x - camera.x;
				screenCoords.y = sprite->y - camera.y;
				//width and height of the sprite
				screenCoords.w = sprite->width;
				screenCoords.h = sprite->height;

				//reminder: all rectangle based textures locations are measured on the top left corner.
				/* o is the top left corner of the rectangle, the 3 other corners aren't calculated.
					o---------------x
					|				|
					|				|
					|				|
					x---------------x
				*/

				//this is an optimization for rendering since rendering many textures offscreen each frame is expensive
				if (screenCoords.x >= 0 && screenCoords.x < camera.w
					&& screenCoords.y >= 0 && screenCoords.y < camera.h)
				{
					//render the sprite if it is within camera bounds
					SDL_RenderCopy(render, sprite->texture, NULL, &screenCoords);
				}
				else
				{

					//todo: render images that are partially offscreen.
					//only render tiles where there right corners can be caught within camera bounds.
					SDL_Point rightCorner;//bottom right corner
					rightCorner.x = screenCoords.x + screenCoords.w;
					rightCorner.y = screenCoords.y + screenCoords.h;
					if (rightCorner.x >= 0 && rightCorner.x < camera.w)
					{
						SDL_RenderCopy(render, sprite->texture, NULL, &screenCoords);
					}
					else if (rightCorner.y >= 0 && rightCorner.y < camera.h)
					{
						SDL_RenderCopy(render, sprite->texture, NULL, &screenCoords);
					}
				}

			}
		}
	}
}

void RenderSystem::Draw(SDL_Renderer* render)
{
	SDL_RenderPresent(render);
	SDL_RenderClear(render);
}

SDL_Point RenderSystem::getFloatToIntegerCoordinates(Vect2 position)
{
	SDL_Point p;
	float diffx = position.x - floorf(position.x);
	float diffy = position.y - floorf(position.y);

	p.x = (diffx > 0.5f) ? (int)ceilf(position.x) : (int)floorf(position.x);
	p.y = (diffy > 0.5f) ? (int)ceilf(position.y) : (int)floorf(position.y);

	return p;
}

