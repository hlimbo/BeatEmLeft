#include "RenderSystem.h"
#include "ECS.h"
#include "../Components/Sprite.h"
#include "../Components/Transform.h"
#include <SDL2/SDL_render.h>

#include "../Components/Animation.h"
#include "../Components/SpriteSheet.h"
#include "../Components/Kinematic.h"

RenderSystem::RenderSystem()
{
	ecs = nullptr;
	transformManager = nullptr;
	spriteManager = nullptr;
}

RenderSystem::RenderSystem(ECS* ecs)
{
	this->ecs = ecs;
	transformManager = &(ecs->transforms);
	spriteManager = &(ecs->sprites);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Init(int cameraWidth,int cameraHeight)
{
	camera.x = 0;
	camera.y = 0;
	camera.w = cameraWidth;
	camera.h = cameraHeight;

	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
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
	Transform* transformComponent = transformManager->GetComponent(id);
	Sprite* spriteComponent = spriteManager->GetComponent(id);
	if (transformComponent != nullptr && spriteComponent != nullptr)
	{
		SDL_Point targetPos = getFloatToIntegerCoordinates(transformComponent->position);
		
		//set a camera offset relative to the entity ~ camera centered on target entity
		camera.x = (targetPos.x + spriteComponent->width / 2) - (camera.w / 2);
		camera.y = (targetPos.y + spriteComponent->height / 2) - (camera.h / 2);
		
		//have camera not move for now
		//camera.x = 0;
		//camera.y = 0;

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
void RenderSystem::Update(SDL_Renderer* render)
{

	//camera tracking
	int backgroundID = ecs->entitySystem.GetIDs("Background").at(0);

	int playerID = ecs->entitySystem.GetIDs("Player").at(0);
	SetEntityToFollow(playerID);

	vector<int> ids = ecs->entitySystem.GetIDs();
	for (auto it = ids.begin();it != ids.end();++it)
	{
		Transform* transform = transformManager->GetComponent(*it);
		Sprite* sprite = spriteManager->GetComponent(*it);

		if (transform != nullptr && sprite != nullptr)
		{

			sprite->position = getFloatToIntegerCoordinates(transform->position);
			
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
				screenBounds.x = sprite->position.x - camera.x;
				screenBounds.y = sprite->position.y - camera.y;
				screenBounds.w = sprite->width;
				screenBounds.h = sprite->height;
				SDL_RenderCopy(render, sprite->texture, NULL/* source texture is used if I want to animate the sprite*/, &screenBounds);
			}
			else //for all other sprites... render them  (right now render order happens from lowest entity id to highest entity id)
			{
				//render the sprite if it is seen by the camera (check if sprite is within screen bounds)
				SDL_Rect screenCoords;
				screenCoords.x = sprite->position.x - camera.x;
				screenCoords.y = sprite->position.y - camera.y;
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

	//temp code
	auto playerTransform = ecs->transforms.GetComponent(playerID);
	auto playerKinematic = ecs->kinematics.GetComponent(playerID);
	auto playerAnimation = ecs->animations.GetComponent(playerID);
	if (playerTransform == nullptr || playerKinematic == nullptr || playerAnimation == nullptr)
		return;

	SpriteSheet* currentAnim = playerAnimation->Get("idle");
	SDL_Rect srcRect = currentAnim->PlayAnimation(0.166f, 0.5f);
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	if (playerKinematic->velocity.x != 0.0f && playerKinematic->velocity.y == 0.0f)
	{
		currentAnim = playerAnimation->Get("walk");
		if (playerKinematic->velocity.x < 0.0f)
			flip = SDL_FLIP_HORIZONTAL;
		else
			flip = SDL_FLIP_NONE;
		srcRect = currentAnim->PlayAnimation(0.166f, 0.5f);
	}
	if (playerKinematic->velocity.y != 0.0f)
	{
		currentAnim = playerAnimation->Get("jump");
		if (playerKinematic->velocity.x < 0.0f)
			flip = SDL_FLIP_HORIZONTAL;
		else
			flip = SDL_FLIP_NONE;
		srcRect = currentAnim->PlayAnimation(0.166f, 2.5f);
	}

	//convert game coordinates to screen coordinates
	SDL_Point screenCoords = getFloatToIntegerCoordinates(playerTransform->position);
	SDL_Rect screenBounds;
	screenBounds.x = screenCoords.x - camera.x;
	screenBounds.y = screenCoords.y - camera.y;
	screenBounds.w = 32 * 2;
	screenBounds.h = 64 * 2;
	//SDL_RenderCopy(render, currentAnim->texture, &srcRect, &screenBounds);
	SDL_RenderCopyEx(render, currentAnim->texture, &srcRect, &screenBounds, 0.0f, NULL, flip);
}

void RenderSystem::Draw(SDL_Renderer* render)
{
	SDL_RenderPresent(render);
	SDL_RenderClear(render);
}

//probably pull out as a utility function
SDL_Point RenderSystem::getFloatToIntegerCoordinates(Vect2 position)
{
	return SDL_Point{ (int)roundf(position.x), (int)roundf(position.y) };
}


