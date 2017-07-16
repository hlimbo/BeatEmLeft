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
#include "Utility/MapFileLoader.h"

using namespace std;

//use this function to check if a coordinate is between minCoordinate and maxCoordinate
bool isOnLineSegment(float coordinate, float minCoordinate, float maxCoordinate)
{
	return coordinate > minCoordinate && coordinate < maxCoordinate;
}

int main(int argc, char* argv[])
{
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

	//TileMap Creation ~ 12x12 grid = 480x480 screen
	const int tileWidth = 40;
	const int tileHeight = 40;
	const int rowLength = 12;
	const int colLength = 12;
	int pad = 1;

	//0 = empty spot, 1 = filled spot
	int tileMap[rowLength][colLength] = 
	{
		{ 0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,1,1,1,1,1,1,1,1,0,0 },
		{ 0,0,0,1,1,1,1,1,1,0,0,0 },
		{ 0,0,0,1,1,1,1,1,1,0,0,0 },
		{ 0,0,0,0,1,1,1,1,0,0,0,0 },
		{ 0,0,0,0,0,1,1,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 }

	};

	//map file loader test
	MapFileLoader::TileMap map;
	string mapFilePath = mainPath + string("funky_map.txt");
	MapFileLoader::Load(mapFilePath.c_str(), &map);

	for (int r = 0;r < map.rowCount;++r)
	{
		for (int c = 0;c < map.colCount;++c)
		{
			if (map.contents[r][c] == 1)
			{
				int tileID = ecs.entitySystem.CreateEntity("Tile");
				//construct list of tileCoordinates
				Vect2 tilePosition((float)c * (float)(tileWidth + pad),
					(float)r * (float)(tileHeight + pad));
				auto transform = new Transform(tilePosition);

				auto sprite = new Sprite(store.Get("block.png"));
				sprite->width = tileWidth;
				sprite->height = tileHeight;

				auto boxCollider = new BoxCollider();
				boxCollider->position = tilePosition;
				boxCollider->width = (float)tileWidth;
				boxCollider->height = (float)tileHeight;
			
				ecs.transforms.AddComponent(tileID, transform);
				ecs.sprites.AddComponent(tileID, sprite);
				ecs.boxColliders.AddComponent(tileID, boxCollider);
			}
		}
	}

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


	//here I know that the tileIDs won't change since they don't get destroyed
	vector<int> tileIDs = ecs.entitySystem.GetIDs("Tile");

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

		movementSys.UpdateKinematics(deltaTime);
		
		movementSys.CheckForCollisions(observedDeltaTime);

		//Correct the velocity of the player before calling updatePositions
		//to ensure new position is a valid spot
		//Vect2 newP(playerTransform->position + playerKinematic->velocity);
		//Vect2 oldP(playerTransform->position);
		//Vect2 deltaP(newP - oldP);

		//collision here!
		//for (vector<int>::iterator it = tileIDs.begin();
		//	it != tileIDs.end();
		//	++it)
		//{
		//	Transform* tile = ecs.transforms.GetComponent(*it);
		//	BoxCollider* box = ecs.boxColliders.GetComponent(*it);

		//	if (isOnLineSegment(oldP.y, tile->position.y, tile->position.y + box->height)
		//		|| isOnLineSegment(oldP.y + playerBox->height, tile->position.y, tile->position.y + box->height))
		//	{
		//		//player moving to the right
		//		if (deltaP.x > 0.0f)
		//		{
		//			//left side tile
		//			float timeX = (tile->position.x - (oldP.x + playerBox->width)) / deltaP.x;

		//			//fix numerical round off error
		//			if (fabsf(timeX) < 0.001f)
		//				timeX = 0.0f;

		//			if (timeX >= 0 && timeX <= observedDeltaTime)
		//			{
		//				float adjustedVelX = deltaP.x * timeX;
		//				float contactX = (oldP.x + playerBox->width) + adjustedVelX;
		//				if (newP.x + playerBox->width > contactX)
		//				{
		//					playerKinematic->velocity.x = adjustedVelX;
		//				}
		//			}
		//		}
		//		else if (deltaP.x < 0.0f) //player moving to the left
		//		{
		//			//right side tile
		//			float timeX2 = (oldP.x - (tile->position.x + box->width)) / fabsf(deltaP.x);
		//			//printf("TimeX2: %f\n", timeX2);

		//			if (fabsf(timeX2) < 0.001f)
		//				timeX2 = 0.0f;

		//			if (timeX2 >= 0 && timeX2 <= observedDeltaTime)
		//			{
		//				float contactX2 = oldP.x + (deltaP.x * timeX2);
		//				float rightTileSide = tile->position.x + box->width;
		//				if (newP.x < contactX2)
		//				{
		//					playerKinematic->velocity.x = (deltaP.x * timeX2);
		//				}
		//			}
		//		}
		//	}

		//	if (isOnLineSegment(oldP.x, tile->position.x, tile->position.x + box->width)
		//		|| isOnLineSegment(oldP.x + playerBox->width, tile->position.x, tile->position.x + box->width))
		//	{
		//		if (deltaP.y != 0.0f)
		//		{
		//			//check top side of tile
		//			if (playerKinematic->direction.y > 0.0f)
		//			{
		//				float timeY = (tile->position.y - (oldP.y + playerBox->height)) / deltaP.y;
		//				//printf("TimeY: %f, deltaTime: %f\n", timeY,observedDeltaTime);

		//				//there is some numerical error with the time not being = to 0.0f
		//				if (fabsf(timeY) < 0.001f)
		//					timeY = 0.0f;

		//				if (timeY >= 0 && timeY <= observedDeltaTime)
		//				{
		//					//Note: deltaP.y * timeY is how much the player would need to move to touch the tile.
		//					float contactY = (oldP.y + playerBox->height) + (deltaP.y * timeY);
		//					float topSideTile = tile->position.y;

		//					//printf("contactY: %f\n", contactY);
		//					//printf("topSideTile: %f\n", topSideTile);

		//					if (newP.y + playerBox->height > contactY)
		//					{
		//						//change the velocity instead of correcting the position
		//						playerKinematic->velocity.y = deltaP.y * timeY;
		//					}

		//				}

		//			}

		//			//check bottom side of tile
		//			else if (playerKinematic->direction.y < 0.0f)
		//			{
		//				float timeY2 = (oldP.y - (tile->position.y + box->height)) / fabsf(deltaP.y);
		//				//printf("TimeY2: %f\n", timeY2);
		//				//check for numerical error here where the values don't always return 0.0f e.g. instead returns -0.000003
		//				if(fabsf(timeY2) < 0.001f)
		//					timeY2 = 0.0f;
		//				
		//				if (timeY2 >= 0 && timeY2 <= observedDeltaTime)
		//				{
		//					float contactY2 = oldP.y + (deltaP.y * timeY2);
		//					float bottomTileSide = tile->position.y + box->height;
		//					//printf("contactY2: %f\n", contactY2);
		//					//printf("bottomTileSide: %f\n", bottomTileSide);

		//					if (newP.y < contactY2)
		//					{
		//						playerKinematic->velocity.y = deltaP.y * timeY2;
		//					}
		//				}
		//			}
		//		}
		//	}

		//}


		movementSys.UpdatePositions(deltaTime);
		movementSys.CorrectCollisionOverlaps(observedDeltaTime);

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

	//	printf("fps: %d\n", (int)observedFPS);
	}

	ecs.FreeKeyboard();
	return 0;
}