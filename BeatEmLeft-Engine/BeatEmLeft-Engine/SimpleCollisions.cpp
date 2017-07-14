#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include <Vect2.h>

#include "Systems/RenderSystem.h"
#include "Systems/ComponentManager.h"
#include "Systems/EntitySystem.h"
#include "Components/Sprite.h"
#include "Components/Transform.h"
#include "Components/BoxCollider.h"
#include "Systems/ECS.h"
#include "Core.h"
#include "Systems/MovementSystem.h"

#include "Input/KeyboardController.h"
#include "Input/GameController.h"
#include "Utility/TextureLoader.h"
#include "Utility/TextureStore.h"

using namespace std;

//use this function to check if a coordinate is between minCoordinate and maxCoordinate
bool isOnLineSegment(float coordinate, float minCoordinate, float maxCoordinate)
{
	return coordinate > minCoordinate && coordinate < maxCoordinate;
}

int main(int argc, char* argv[])
{
	//Inits all SDL subsystems, creates window and renderer
	Core core;
	SDL_Renderer* render = core.getRenderer();

	//setup texture file paths
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");
	string playerPath = mainPath + string("blue.png");
	string backgroundPath = mainPath + string("Background.png");
	string tilePath = mainPath + string("block.png");

	TextureStore store(render);
	store.Load("Background.png", backgroundPath);
	store.Load("block.png", tilePath);
	store.Load("blue.png", playerPath);

	ECS ecs;

	//entity component creation
	
	//background entity creation
	int bgID = ecs.entitySystem.CreateEntity("Background");
	auto bgTransform = new Transform();
	bgTransform->position = Vect2(0.0f, 0.0f);	
	auto bgSprite = new Sprite(store.Get("Background.png"));
	ecs.transforms.AddComponent(bgID, bgTransform);
	ecs.sprites.AddComponent(bgID, bgSprite);

	//tile entity creation
	int tileID = ecs.entitySystem.CreateEntity("Tile");
	auto tileTransform = new Transform();
	tileTransform->position = Vect2(545.5f,322.4f);
	auto tileSprite = new Sprite(store.Get("block.png"));
	tileSprite->width = 100;
	tileSprite->height = 100;
	auto tileBox = new BoxCollider();
	tileBox->position = tileTransform->position;
	tileBox->width = (float)tileSprite->width;
	tileBox->height = (float)tileSprite->height;
	ecs.transforms.AddComponent(tileID, tileTransform);
	ecs.sprites.AddComponent(tileID, tileSprite);
	ecs.boxColliders.AddComponent(tileID, tileBox);

	//player entity creation
	int playerID = ecs.entitySystem.CreateEntity("Player");
	auto playerTransform = new Transform();
	playerTransform->position = Vect2(20.0f, 35.3f);
	auto playerSprite = new Sprite(store.Get("blue.png"));
	playerSprite->width = 40;
	playerSprite->height = 40;
	auto playerKinematic = new Kinematic();
	playerKinematic->minSpeed = 100.0f;
	playerKinematic->maxSpeed = 552.3f;
	playerKinematic->currentSpeed = playerKinematic->minSpeed;
	playerKinematic->direction = Vect2(0.0f, 0.0f);//direction depends on what key is pressed
	playerKinematic->accelFactor = 1.7f;
	auto playerBox = new BoxCollider();
	playerBox->position = playerTransform->position;
	playerBox->width = (float)playerSprite->width;
	playerBox->height = (float)playerSprite->height;

	ecs.transforms.AddComponent(playerID, playerTransform);
	ecs.sprites.AddComponent(playerID, playerSprite);
	ecs.kinematics.AddComponent(playerID, playerKinematic);
	ecs.boxColliders.AddComponent(playerID, playerBox);
	ecs.InitKeyboard();
	KeyboardController* keyboard = ecs.RegisterKeyboard(playerID);
	
	//initialize systems after components have been initialized.
	RenderSystem renderSys(&ecs);
	renderSys.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
	MovementSystem movementSys(&ecs);
	movementSys.Init();


	//debug for collisions
	int yCollisions = 0;
	int yCollisions2 = 0;

	//---------------- Game Loop ------------------//

	//observedDeltaTime is measured in milliseconds
	float observedDeltaTime = core.getTargetDeltaTime();
	float deltaTime = observedDeltaTime / 1000.0f;//converts from milliseconds to seconds
	//to avoid unnecessary context switches os might do (which I have no control over.. cache the target delta time)
	float targetDeltaTime = core.getTargetDeltaTime();
	Uint64 observedFPS = core.getTargetFPS();
	float currentTime = 0.0f;
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
	bool running = true;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;

			keyboard->HandleInput(event);
		}

		//movement
		movementSys.UpdateKinematics(deltaTime);
		movementSys.UpdatePositions(deltaTime);

		//collision
		//using the time equations to determine when a point touches a wall
		//https://hero.handmade.network/episode/code/day047#1296

		Vect2 newP(playerTransform->position);
		Vect2 oldP = newP - playerKinematic->velocity;
		Vect2 tilePos(tileTransform->position);

		//top and left sides
		Vect2 deltaP = newP - oldP;

		float timeX = 0.0f;
		float timeY = 0.0f;

		//check the time when player upper left point touches left wall of tile
		//this frame. (time ranges from 0 to 16.66 ms when at 60 fps)
		//derived from the fact that:
		/*
			Goal is to solve for t :
			p(t) = p0 + t * d; //where t = time, d = newPlayerPos - oldPlayerPos
			//and p0 = oldPlayerPos and p(t) = newPlayerPos
			wallPos = p0 + t *d;

			wallPos.x = p0.x + t * d.x;
			wallPos.y = p0.y + t * d.y;

			//assuming that the walls have either undefined slope or 0 slope

		*/

		if (isOnLineSegment(oldP.y, tilePos.y, tilePos.y + tileBox->height)
			|| isOnLineSegment(oldP.y + playerBox->height,tilePos.y,tilePos.y + tileBox->height))
		{
			if (deltaP.x != 0.0f)
			{
				if (playerKinematic->direction.x > 0.0f)
				{
					//left side tile
					timeX = (tilePos.x - (oldP.x + playerBox->width)) / deltaP.x;
					//printf("TimeX: %f ms\n", timeX);

					//Note last check is for numerical error
					if ((timeX >= 0 && timeX <= observedDeltaTime) || fabsf(timeX) < 0.001f)
					{
						//Note: deltaP.x * timeX is how much the player would need to move to touch the tile.
						float contactX = (oldP.x + playerBox->width) + (deltaP.x * timeX);
						//right side of player colliding with left side of tile
						if (newP.x + playerBox->width > contactX)
						{
							float penX = newP.x - contactX;
							newP.x -= penX + playerBox->width;
							playerTransform->position.x = newP.x;
						}
					}

					if (fabsf(timeX) < 0.001f)
					{
						if(timeX != 0.0f)
							printf("timeX: %f\n", timeX);
					}
				}
				else if (playerKinematic->direction.x < 0.0f)
				{
					//right side tile
					float timeX2 = (oldP.x - (tilePos.x + tileBox->width)) / fabsf(deltaP.x);
					//printf("TimeX2: %f\n", timeX2);
					if ((timeX2 >= 0 && timeX2 <= observedDeltaTime) || fabsf(timeX2) < 0.001f)
					{
						float contactX2 = oldP.x + (deltaP.x * timeX2);
						float rightTileSide = tileTransform->position.x + tileBox->width;
						//printf("contactX2: %f\n", contactX2);
						//printf("rightTileSide: %f\n", rightTileSide);
						if (newP.x < contactX2)
						{
							float penX = contactX2 - newP.x;
							newP.x += penX;
							playerTransform->position.x = newP.x;
						}
					}

					//debug to catch round off error
					if (fabsf(timeX2) < 0.001f)
					{
						if(timeX2 != 0.0f)
							printf("timeX2: %f\n", timeX2);
					}
				}
				
			}
		}

		if (isOnLineSegment(oldP.x, tilePos.x, tilePos.x + tileBox->width)
			|| isOnLineSegment(oldP.x + playerBox->width, tilePos.x, tilePos.x + tileBox->width))
		{
			if (deltaP.y != 0.0f)
			{
				//check top side of tile
				if (playerKinematic->direction.y > 0.0f)
				{
					timeY = (tilePos.y - (oldP.y + playerBox->height)) / deltaP.y;
					//printf("TimeY: %f, deltaTime: %f\n", timeY,observedDeltaTime);
					
					//there is some numerical error with the time not being = to 0.0f
					if ((timeY >= 0 && timeY <= observedDeltaTime) || fabsf(timeY) < 0.001f)
					{
						//Note: deltaP.y * timeY is how much the player would need to move to touch the tile.
						float contactY = (oldP.y + playerBox->height) + (deltaP.y * timeY);
						float topSideTile = tilePos.y;

						//printf("contactY: %f\n", contactY);
						//printf("topSideTile: %f\n", topSideTile);

						if (newP.y + playerBox->height > contactY)
						{
							//puts("not enough");
							float penY = (newP.y + playerBox->height) - contactY;

							//printf("penY1: %f\n", penY);
							//printf("velocity: (%f, %f)\n", playerKinematic->velocity.x, playerKinematic->velocity.y);
							newP.y -= penY;

							if (newP.y + playerBox->height > contactY)
							{
								printf("player bot: %f | tile top: %f\n", newP.y + playerBox->height, contactY);
							}

							playerTransform->position.y = newP.y;

						}

					}

					//DEBUG
					if (fabsf(timeY) < 0.001f)
					{
						if (timeY != 0.0f)
						{
							printf("Number of consectutive y collisions: %d\n", yCollisions);
							yCollisions = 0;
							printf("timeY: %f\n", timeY);
						}
						else
							yCollisions++;
					}

				}

				//check bottom side of tile
				else if (playerKinematic->direction.y < 0.0f)
				{
					float timeY2 = (oldP.y - (tilePos.y + tileBox->height)) / fabsf(deltaP.y);
					//printf("TimeY2: %f\n", timeY2);
					//check for numerical error here where the values don't always return 0.0f e.g. instead returns -0.000003
					if ((timeY2 >= 0 && timeY2 <= observedDeltaTime) || fabsf(timeY2) < 0.001f)
					{
						float contactY2 = oldP.y + (deltaP.y * timeY2);
						float bottomTileSide = tilePos.y + tileBox->height;
						//printf("contactY2: %f\n", contactY2);
						//printf("bottomTileSide: %f\n", bottomTileSide);
					
						if (newP.y < contactY2)
						{
							float penY2 = contactY2 - newP.y;
							//printf("penY2: %f\n", penY2);
							//printf("velocity: (%f, %f)\n", playerKinematic->velocity.x, playerKinematic->velocity.y);

							newP.y += penY2;
							playerTransform->position.y = newP.y;
						}
					}

					//Debug
					if (fabsf(timeY2) < 0.001f)
					{
						if (timeY2 != 0.0f)
						{
							printf("Number of consecutive y2 collisions: %d\n", yCollisions2);
							yCollisions2 = 0;
							printf("timeY2: %f\n", timeY2);
						}
						else
						{
							yCollisions2++;
						}
					}
				}
			}
		}

		//drawing
		renderSys.Update(render);
		renderSys.Draw(render);

		endCount = SDL_GetPerformanceCounter();
		observedDeltaTime = (1000.0f * (endCount - startCount)) / performanceFrequency;//gives ms
		observedFPS = performanceFrequency / (endCount - startCount);

		//if the computer can process the update and draw functions faster than 60 fps...
		//cap the frame-rate here to ensure that all computers play roughly around the same fps
		float msDifference = targetDeltaTime - observedDeltaTime;
		if (msDifference > 0)
		{
			SDL_Delay((Uint32)msDifference);
			//Note: must re-record the times after the delay since the times before the delay maybe
			//under 16.666 ms
			endCount = SDL_GetPerformanceCounter();
			observedDeltaTime = (1000.0f * (endCount - startCount)) / performanceFrequency;//gives ms
			observedFPS = performanceFrequency / (endCount - startCount);
		}

		/*
			Side Note:
			if(msDifference == 0) this means that the computer ran all the update and draw instructions
			exactly 16.666 ms

			if(msDifference < 0).. this means that it took the computer to run the update and draw
			functions longer this frame, resulting in a drop in fps this frame. If 

		*/

		//-----------------------------------------------------

		currentTime += observedDeltaTime;
		deltaTime = observedDeltaTime / 1000.0f;
		startCount = endCount;

		//display fps text in title
		if ((int)currentTime % 100 == 0)
		{

			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}

		//printf("fps: %d\n", (int)observedFPS);
	}

	ecs.FreeKeyboard();
	return 0;
}
