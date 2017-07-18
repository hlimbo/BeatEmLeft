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
#include "Components/SpriteSheet.h"
#include "Systems/ECS.h"
#include "Core.h"
#include "Systems/MovementSystem.h"

#include "Input/KeyboardController.h"
#include "Input/GameController.h"
#include "Utility/TextureLoader.h"
#include "Utility/TextureStore.h"
#include "Utility/MapFileLoader.h"

using namespace std;

void CalcCirclePoints(SDL_Point* points, int startIndex, int cx, int cy, int x, int y)
{
	points[startIndex] = SDL_Point{ cx - x, cy - y };
	points[startIndex + 1] = SDL_Point{ cx - y, cy - x };
	points[startIndex + 2] = SDL_Point{ cx + y, cy - x };
	points[startIndex + 3] = SDL_Point{ cx + x, cy - y };
	points[startIndex + 4] = SDL_Point{ cx - x, cy + y };
	points[startIndex + 5] = SDL_Point{ cx - y, cy + x };
	points[startIndex + 6] = SDL_Point{ cx + y, cy + x };
	points[startIndex + 7] = SDL_Point{ cx + x, cy + y };
}

int DrawCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
	//draw the circle here using Bresenham’s Algorithm
	int p = 0;
	int q = radius;
	int numPixelsToDraw = q * 8;
	SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * numPixelsToDraw);
	//d = decision parameter which acts as a function
	//which helps decide to draw the next pixel to its horizontally adjacent neighbor or its diagonally adjacent neighbor
	int d = 3 - 2 * radius;

	while (p < q)
	{
		//slightly better optimization:: batch all points into one draw call ~ draw calls are expensive
		CalcCirclePoints(points, p * 8, cx, cy, p, q);
		++p;
		if (d < 0)
		{
			d = d + (4 * p) + 6;
		}
		else
		{
			d = d + (4 * (p - q)) + 10;
			--q;
		}
	}

	int result = SDL_RenderDrawPoints(renderer, points, numPixelsToDraw);
	free(points);
	return result;
}

int DrawCircle(SDL_Renderer* renderer, SDL_Point center, int radius)
{
	return DrawCircle(renderer, center.x, center.y, radius);
}


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
	string sheetPath = mainPath + string("treyArt.png");

	TextureStore store(render);
	store.Load("Background.png", backgroundPath);
	store.Load("block.png", tilePath);
	store.Load("blue.png", playerPath);
	store.Load("treyArt.png", sheetPath);

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

	//temp
	//Transform* tileTransform = nullptr;
	//Sprite* tileSprite = nullptr;
	//CircleCollider* tileCircle = nullptr;

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

				//temp
				auto circleCollider = new CircleCollider();
				circleCollider->position = tilePosition;
				circleCollider->radius = (float)tileWidth;
			
				ecs.transforms.AddComponent(tileID, transform);
				ecs.sprites.AddComponent(tileID, sprite);
				ecs.boxColliders.AddComponent(tileID, boxCollider);
				ecs.circleColliders.AddComponent(tileID, circleCollider);

				//temp
			//	tileTransform = transform;
			//	tileSprite = sprite;
			//	tileCircle = circleCollider;
			}
		}
	}

	//player entity creation
	int playerID = ecs.entitySystem.CreateEntity("Player");
	auto playerTransform = new Transform();
	playerTransform->position = Vect2(20.0f, 0.0f);
	
	auto playerSprite = new Sprite(store.Get("blue.png"));
	playerSprite->width = 40;
	playerSprite->height = 40;
	
	auto playerKinematic = new Kinematic();
	playerKinematic->minSpeed = 155.0f;
	playerKinematic->maxSpeed = 363.2f;
	playerKinematic->currentSpeed = playerKinematic->minSpeed;
	playerKinematic->direction = Vect2(0.0f, 0.0f);//direction depends on what key is pressed
	playerKinematic->accelFactor = 1.7f;
	//playerKinematic->acceleration = Vect2(0.0f, 0.0f);//not really using this...
	
	//gravity
	playerKinematic->gravity = 75.4f;
	playerKinematic->minGravity = 75.4f;
	playerKinematic->maxGravity = 492.5f;
	playerKinematic->gravityFactor = 3.6f;

	//jumping
	playerKinematic->jumpSpeed = 85.6f;
	playerKinematic->minJumpSpeed = 85.6f;
	playerKinematic->maxJumpSpeed = 495.0f;
	playerKinematic->jumpFactor = 10.5f;
	playerKinematic->currentJumpTime = 0.0f;
	playerKinematic->maxJumpTime = 0.65f;

	float someDiff = (playerKinematic->jumpSpeed * playerKinematic->jumpFactor) - (playerKinematic->gravity * playerKinematic->gravityFactor);
	float someTime = 0.0f;
	if (someDiff != 0.0f)
		someTime = ((playerKinematic->gravity - playerKinematic->jumpSpeed) / someDiff) * 1000.0f;
	
	//printf("some time: %f\n", someTime);

	//temp
	SpriteSheet spriteSheet;
	SDL_Texture* srcTexture = store.Get("treyArt.png");
	spriteSheet.scaleX = 0.5f;
	spriteSheet.scaleY = 0.5f;
	spriteSheet.SetTextureAttributes(srcTexture, 300, 450);
	int frameIndex = 0;
	float leftSwitchDelayTime = 0.0f;
	float rightSwitchDelayTime = 0.0f;
	float maxDelayTime = 0.02785f;//measured in seconds

	auto playerBox = new BoxCollider();
	playerBox->position = playerTransform->position;
	playerBox->width = (float)playerSprite->width;
	playerBox->height = (float)playerSprite->height;

	auto playerCircle = new CircleCollider();
	playerCircle->position = playerTransform->position;
	playerCircle->radius = (float)playerSprite->width;

	ecs.transforms.AddComponent(playerID, playerTransform);
	ecs.sprites.AddComponent(playerID, playerSprite);
	ecs.kinematics.AddComponent(playerID, playerKinematic);
	ecs.boxColliders.AddComponent(playerID, playerBox);
	ecs.circleColliders.AddComponent(playerID, playerCircle);
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

		keyboard->UpdateKeyStates(observedDeltaTime);

		movementSys.UpdateKinematics(deltaTime);
		movementSys.CheckForCollisions(observedDeltaTime);

		movementSys.UpdatePositions(deltaTime);
		movementSys.CorrectCollisionOverlaps(observedDeltaTime);

		
		//don't need for now
		//movementSys.CheckForCircleCollisions(observedDeltaTime);

		renderSys.Update(render);

		////temporary draw circle bounds tile
		//SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
		//SDL_Point center = SDL_Point{ (int)roundf(tileTransform->position.x),(int)roundf(tileTransform->position.y) };
		//DrawCircle(render, center, (int)tileCircle->radius);

		////temporary draw circle bounds player
		//SDL_SetRenderDrawColor(render, 0, 0, 255, 255);
		//SDL_Point pcenter = SDL_Point{ (int)roundf(playerTransform->position.x),(int)roundf(playerTransform->position.y) };
		//DrawCircle(render, pcenter, (int)playerCircle->radius);

		//temp draw spriteSheet
		//if (keyboard->KeyPressed("a"))
		//{
		//	if (leftSwitchDelayTime > maxDelayTime)
		//	{
		//		frameIndex = (frameIndex + 1) % spriteSheet.GetFrameCount();
		//		leftSwitchDelayTime = 0.0f;
		//	}
		//	else
		//	{
		//		leftSwitchDelayTime += deltaTime;
		//	}
		//}
		//if (keyboard->KeyPressed("d"))
		//{
		//	if (rightSwitchDelayTime > maxDelayTime)
		//	{
		//		if (--frameIndex < 0)
		//		{
		//			frameIndex = spriteSheet.GetFrameCount() - 1;
		//		}
		//		rightSwitchDelayTime = 0.0f;
		//	}
		//	else
		//	{
		//		rightSwitchDelayTime += deltaTime;
		//	}
		//}

		spriteSheet.PlayAnimation(deltaTime, 0.5f);

		//const SDL_Rect* frame = spriteSheet.GetFrame(frameIndex);
		const SDL_Rect frame = spriteSheet.GetCurrentFrame();
		SDL_Rect bounds;
		bounds.x = 255;
		bounds.y = 305;
		bounds.h = 350 * 0.5;
		bounds.w = 450 * 0.5;
		SDL_Texture* sheetTexture = spriteSheet.texture;

		SDL_RenderCopy(render, sheetTexture, &frame, spriteSheet.GetBounds());

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
	//	printf("deltaTime: %f\n", observedDeltaTime);
	}

	ecs.FreeKeyboard();

	return 0;
}