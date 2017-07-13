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
	tileTransform->position = Vect2(250.0f, 314.8f);
	auto tileSprite = new Sprite(store.Get("block.png"));
	tileSprite->width = 50;
	tileSprite->height = 50;
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
	playerTransform->position = Vect2(0.0f, 0.0f);
	auto playerSprite = new Sprite(store.Get("blue.png"));
	playerSprite->width = 40;
	playerSprite->height = 40;
	auto playerKinematic = new Kinematic();
	playerKinematic->minSpeed = 100.0f;
	playerKinematic->maxSpeed = 552.3f;
	playerKinematic->currentSpeed = playerKinematic->minSpeed;
	playerKinematic->direction = Vect2(0.0f, 0.0f);//direction depends on what key is pressed
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


		//should we do pixel perfect collisions or collisions based on what appears in the game world?

		//1. pixel perfect collisions
		/*
		a. convert game world coordinates to pixel coordinates
		b. check those pixel coordinates for collision
		c. apply collision logic to pixel coordinates
		d. convert pixel coordinates back to game world coordinates
		*/
		//2. game world collisions ** (prefer to do this way)
		/*
		a. check floating point coordinates for collisions
		b. apply collision logic to those game world coordinates
		c. convert to pixel coordinates only when drawing to screen
		*/

		//reason why we use floating point coordinates as opposed to pixel coordinates (integers)
		//is to gain more numbers to use!
		//1. using floats as units of movement per update gives the illusion of an object moving
		//at 0.1 pixels per second when in reality pixels can only move exactly 1 pixel per frame!
		//(e.g. cannot move 1 pixel half a pixel away since pixels are modeled as discrete data)
		//2. using floats appears as if a sprite is moving more smoothly than if we were to use
		//ints to calculate motion.
		//3. if you wanted to model curve like motion..it would be really hard to do so using only ints

		//collision
		
		//solve if point is outside the box
	/*	Vect2 playerPos(playerTransform->position);
		Vect2 tilePos(tileTransform->position);

		bool IsPointOutsideBox = playerPos.x < tilePos.x || playerPos.x > tilePos.x + tileBox->width
			|| playerPos.y < tilePos.y || playerPos.y > tilePos.y + tileBox->height;

		bool IsPointInsideBox = !IsPointOutsideBox;

		if (IsPointInsideBox)
		{
			puts("inside box");
		}*/

		//solve if a line is inside of a box
		//Vect2 p(playerTransform->position);
		//float pWidth = playerBox->width;
		//float pHeight = playerBox->height;
		//Vect2 t(tileTransform->position);
		//float tWidth = tileBox->width;
		//float tHeight = tileBox->height;

		//check if 2 boxes are intersecting
	/*	if (fabsf(p.x - t.x) * 2 < pWidth + tWidth && fabsf(p.y - t.y) * 2 < pHeight + tHeight)
		{
			puts("boxes intersect");
		}*/

		////check if player right line is inside box
		//if (p.x + pWidth > t.x && p.x + pWidth < t.x + tWidth)
		//{
		//	if (p.y > t.y && p.y < t.y + tHeight)
		//	{
		//		puts("upper right");
		//	}

		//	if (p.y + pHeight > t.y && p.y + pHeight < t.y + tHeight)
		//	{
		//		puts("lower right");
		//	}
		//}

		////check if player left line is inside box
		//if (p.x > t.x && p.x < t.x + tWidth)
		//{
		//	if (p.y > t.y && p.y < t.y + tHeight)
		//	{
		//		puts("upper left");
		//	}

		//	if (p.y + pHeight > t.y && p.y + pHeight < t.y + tHeight)
		//	{
		//		puts("lower left");
		//	}
		//}

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

		//check left side of tile
		if (isOnLineSegment(oldP.y, tilePos.y, tilePos.y + tileBox->height)
			|| isOnLineSegment(oldP.y + playerBox->height,tilePos.y,tilePos.y + tileBox->height))
		{
			if (deltaP.x != 0.0f)
			{
				if (playerKinematic->direction.x > 0.0f)
				{
					//left side tile
					timeX = (tilePos.x - (oldP.x + playerBox->width)) / deltaP.x;
					printf("TimeX: %f ms\n", timeX);
					if (timeX >= 0 && timeX <= observedDeltaTime)
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
				}

				if (playerKinematic->direction.x < 0.0f)
				{
					//right side tile
					float timeX2 = (oldP.x - (tilePos.x + tileBox->width)) / fabsf(deltaP.x);
					printf("TimeX2: %f\n", timeX2);
					if (timeX2 >= 0 && timeX2 <= observedDeltaTime)
					{
						float contactX2 = oldP.x + (deltaP.x * timeX2);
						float rightTileSide = tileTransform->position.x + tileBox->width;
						printf("contactX2: %f\n", contactX2);
						printf("rightTileSide: %f\n", rightTileSide);
						if (newP.x < contactX2)
						{
							float penX = contactX2 - newP.x;
							newP.x += penX;
							playerTransform->position.x = newP.x;
						}
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
					printf("TimeY: %f ms\n", timeY);
					if (timeY >= 0 && timeY <= observedDeltaTime)
					{
						//Note: deltaP.y * timeY is how much the player would need to move to touch the tile.
						float contactY = (oldP.y + playerBox->height) + (deltaP.y * timeY);
						//bottom side of player is touching the top side of tile
						if (newP.y + playerBox->height > contactY)
						{
							float penY = newP.y - contactY;
							newP.y -= penY + playerBox->height;
							playerTransform->position.y = newP.y;
						}
					}
				}

				//check bottom side of tile
				if (playerKinematic->direction.y < 0.0f)
				{
					float timeY2 = (oldP.y - (tilePos.y + tileBox->height)) / fabsf(deltaP.y);
					printf("TimeY2: %f\n", timeY2);
					if (timeY2 >= 0 && timeY2 <= observedDeltaTime)
					{
						float contactY2 = oldP.y + (deltaP.y * timeY2);
						float bottomTileSide = tilePos.y + tileBox->height;
						printf("contactY2: %f\n", contactY2);
						printf("bottomTileSide: %f\n", bottomTileSide);
						
						if (newP.y < contactY2)
						{
							float penY2 = contactY2 - newP.y;
							newP.y += penY2;
							playerTransform->position.y = newP.y;
						}
					}
				}
			}
		}

		////this ensures it only does the check if playerPoint is between a bounded line
		//if (oldP.y >= tilePos.y && oldP.y <= tilePos.y + tileBox->height)
		//{
		//	//calculates the time it will take in milliseconds 
		//	//when player will touch left wall of box
		//	if (deltaP.x != 0.0f)
		//	{
		//		timeX = (tilePos.x - oldP.x) / deltaP.x;
		//		printf("TimeX: %f\n", timeX);

		//		//if player is not moving backwards and the amount of time it will
		//		//take when the player touches the wall is within the timeStep (e.g. within this update frame)"
		//		//notify the player that a collision can happen.
		//		if (timeX >= 0 && timeX <= observedDeltaTime)
		//		{
		//		//	puts("collision can happen within this frame");

		//			//print out the point where the collision will happen on the wall
		//			Vect2 collisionPoint;
		//			collisionPoint = oldP + (deltaP * timeX);
		//			printf("Collision at wall left: (%f, %f)\n",collisionPoint.x ,collisionPoint.y);
		//			printf("NewP: (%f, %f)\n", newP.x, newP.y);

		//			if (newP.x + playerBox->width > collisionPoint.x)
		//			{
		//				float penX = newP.x - collisionPoint.x;
		//				printf("collision will happen this frame: penX %f\n",penX);
		//				
		//				//correct the collision here//jitters weirdly here when moving from right to left
		//				newP.x -= penX + playerBox->width;
		//				playerTransform->position.x = newP.x;
		//			}
		//			else
		//			{
		//				puts("collision will not happen this frame");
		//			}
		//		}
		//	}
		//}

		////this ensures it only does the check if playerPoint is between a bounded line
		//if (oldP.x >= tilePos.x && oldP.x <= tilePos.x + tileBox->width)
		//{
		//	//calculates the time it will take in milliseconds
		//	//when player will touch the top wall of box
		//	if (deltaP.y != 0.0f)
		//	{
		//		timeY = (tilePos.y - oldP.y) / deltaP.y;
		//		printf("TimeY: %f\n", timeY);

		//		if (timeY >= 0 && timeY <= observedDeltaTime)
		//		{
		//			Vect2 collisionPoint;
		//			collisionPoint = oldP + (deltaP * timeY);
		//			printf("Collision at wall top: (%f, %f)\n", collisionPoint.x, collisionPoint.y);
		//			printf("NewP: (%f, %f)\n", newP.x, newP.y);
		//			if (newP.y + playerBox->height > collisionPoint.y)
		//			{
		//				float penY = newP.y - collisionPoint.y;
		//				//correct collision
		//				newP.y -= penY + playerBox->height;
		//				playerTransform->position.y = newP.y;
		//			}
		//		}
		//	}
		//}

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
