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
#include "Components/Animation.h"
#include "Components/SlopeCollider.h"
#include "Systems/ECS.h"
#include "Core.h"
#include "Systems/MovementSystem.h"

#include "Input/KeyboardController.h"
#include "Input/GameController.h"
#include "Utility/TextureLoader.h"
#include "Utility/TextureStore.h"
#include "Utility/MapFileLoader.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	//setup texture file paths
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	string idlePath = mainPath + string("adv_idle.png");
	string walkPath = mainPath + string("adv_walk.png");
	string jumpPath = mainPath + string("adv_jump.png");

	string backgroundPath = mainPath + string("Background.png");
	string tilePath = mainPath + string("block.png");
	string playerPath = mainPath + string("redblock.png");
	string slopePath = mainPath + string("slope.png");
	string slopeRevPath = mainPath + string("slope_rev.png");
	string whitePath = mainPath + string("white.png");

	TextureStore store(render);
	store.Load("Background.png", backgroundPath);
	store.Load("block.png", tilePath);
	store.Load("adv_idle.png", idlePath);
	store.Load("adv_walk.png", walkPath);
	store.Load("adv_jump.png", jumpPath);
	store.Load("redblock.png", playerPath);
	store.Load("slope.png", slopePath);
	store.Load("slope_rev.png", slopeRevPath);
	store.Load("white.png", whitePath);

	store.SetAlpha("white.png", 255 / 2);

	MapFileLoader::TileMap map;
	string mapFilePath = mainPath + string("funky_map.txt");
	MapFileLoader::Load(mapFilePath.c_str(), &map);

	ECS ecs;

	int bgID = ecs.entitySystem.CreateEntity("Background");
	auto bgTransform = new Transform();
	bgTransform->position = Vect2(0.0f, 0.0f);
	auto bgSprite = new Sprite(store.Get("Background.png"));
	ecs.transforms.AddComponent(bgID, bgTransform);
	ecs.sprites.AddComponent(bgID, bgSprite);

	float tileWidth = (float)map.tileWidth;
	float tileHeight = (float)map.tileHeight;
	//int pad = 1;
	//tiles initialization
	for (int r = 0;r < map.rowCount;++r)
	{
		for (int c = 0;c < map.colCount;++c)
		{
			if (map.contents[r][c] == 1) //block tiles
			{
				int tileID = ecs.entitySystem.CreateEntity("Tile");
				//construct list of tileCoordinates
				Vect2 tilePosition((float)c * (float)(tileWidth),
					(float)r * (float)(tileHeight));
				auto transform = new Transform(tilePosition);

				auto sprite = new Sprite(store.Get("block.png"));
				sprite->width = (int)tileWidth;
				sprite->height = (int)tileHeight;

				auto boxCollider = new BoxCollider();
				boxCollider->position = tilePosition;
				boxCollider->width = (float)tileWidth;
				boxCollider->height = (float)tileHeight;

				ecs.transforms.AddComponent(tileID, transform);
				ecs.sprites.AddComponent(tileID, sprite);
				ecs.boxColliders.AddComponent(tileID, boxCollider);
			}
			else if (map.contents[r][c] == 2) //slope tiles
			{
				int slopeID = ecs.entitySystem.CreateEntity("Slope");

				Vect2 tilePosition((float)c * (float)(tileWidth),
					(float)r * (float)(tileHeight));
				auto transform = new Transform(tilePosition);

				auto sprite = new Sprite(store.Get("slope.png"));
				sprite->width = (int)tileWidth;
				sprite->height = (int)tileHeight;

				auto slopeCollider = new SlopeCollider(tilePosition);
				slopeCollider->width = tileWidth;
				slopeCollider->height = tileHeight;

				ecs.transforms.AddComponent(slopeID, transform);
				ecs.sprites.AddComponent(slopeID, sprite);
				ecs.slopes.AddComponent(slopeID, slopeCollider);
			
				//debug
				printf("slope position: (%f,%f)\n", tilePosition.x, tilePosition.y);
				printf("slope m: %f\n", slopeCollider->GetSlope());
			}
			else if (map.contents[r][c] == 3) //slope reverse tiles
			{
				int slopeID = ecs.entitySystem.CreateEntity("SlopeRev");

				Vect2 tilePosition((float)c * (float)(tileWidth),
					(float)r * (float)(tileHeight));
				auto transform = new Transform(tilePosition);

				auto sprite = new Sprite(store.Get("slope_rev.png"));
				sprite->width = (int)tileWidth;
				sprite->height = (int)tileHeight;

				auto slopeCollider = new SlopeCollider(tilePosition);
				slopeCollider->width = tileWidth;
				slopeCollider->height = tileHeight;

				ecs.transforms.AddComponent(slopeID, transform);
				ecs.sprites.AddComponent(slopeID, sprite);
				ecs.slopes.AddComponent(slopeID, slopeCollider);
			}
		}
	}

	int playerID = ecs.entitySystem.CreateEntity("Player");
	
	auto playerTransform = new Transform();
	playerTransform->position = Vect2(20.0f, 0.0f);
	
	//temp
	SDL_Point offset{ 7, 1 };

	//Sprite
	auto playerSprite = new Sprite(store.Get("redblock.png"));
	playerSprite->width = 20;
	playerSprite->height = 90;
	playerSprite->position.x = (int)playerTransform->position.x + offset.x;
	playerSprite->position.y = (int)playerTransform->position.y + offset.y;

	//SpriteSheets / Animations
	auto playerAnimation = new Animation();
	playerAnimation->Add("idle", new SpriteSheet(store.Get("adv_idle.png"), 32, 64));
	playerAnimation->Add("walk", new SpriteSheet(store.Get("adv_walk.png"), 32, 64));
	playerAnimation->Add("jump", new SpriteSheet(store.Get("adv_jump.png"), 32, 64));

	//experiment change alpha to a portion of any spritesheet in playerAnimation
	playerAnimation->Get("walk")->SetAlpha(0, 100);
	//playerAnimation->Get("walk")->SetAlpha(1, 255);
	//playerAnimation->Get("walk")->SetAlpha(2, 100);
	//playerAnimation->Get("walk")->SetAlpha(3, 255);
	//playerAnimation->Get("walk")->SetAlpha(4, 100);
	//playerAnimation->Get("walk")->SetAlpha(5, 255);
	//playerAnimation->Get("walk")->SetAlpha(SDL_BlendMode::SDL_BLENDMODE_BLEND,125);

	auto playerKinematic = new Kinematic();
	playerKinematic->minSpeed = 100.0f;
	playerKinematic->maxSpeed = 150.0f;
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
	playerKinematic->jumpSpeed = 110.6f;    
	playerKinematic->minJumpSpeed = 110.6f;
	playerKinematic->maxJumpSpeed = 550.5f;
	playerKinematic->jumpFactor = 10.5f;
	playerKinematic->currentJumpTime = 0.0f;
	playerKinematic->maxJumpTime = 0.65f;

	auto playerBox = new BoxCollider();
	playerBox->position = playerTransform->position;
	playerBox->width = 20;
	playerBox->height = 90;

	ecs.transforms.AddComponent(playerID, playerTransform);
	ecs.kinematics.AddComponent(playerID, playerKinematic);
	ecs.boxColliders.AddComponent(playerID, playerBox);
	ecs.animations.AddComponent(playerID, playerAnimation);
	ecs.sprites.AddComponent(playerID, playerSprite);
	ecs.InitKeyboard();
	KeyboardController* keyboard = ecs.RegisterKeyboard(playerID);

	//initialize systems
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

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					//screen to game world coordinates as ints
					SDL_Point gameCoords;
					gameCoords.x = renderSys.camera.x + event.button.x;
					gameCoords.y = renderSys.camera.y + event.button.y;
					printf("left mouse click (%d,%d)\n", event.button.x, event.button.y);
					printf("game world coords(%d, %d)\n", gameCoords.x, gameCoords.y);
				}
			}

			keyboard->HandleInput(event);
		}

		keyboard->UpdateKeyStates(observedDeltaTime);

		movementSys.UpdateKinematics(deltaTime);
		movementSys.CheckForCollisions(observedDeltaTime);
		movementSys.CheckForSlopes(observedDeltaTime);
		movementSys.UpdatePositions(deltaTime);
		movementSys.CorrectCollisionOverlaps(observedDeltaTime);

		renderSys.Update(render);

		SDL_Rect dstRect;
		dstRect.h = 64;
		dstRect.w = 64;
		dstRect.x = 0;
		dstRect.y = 384;
		SDL_RenderCopy(render, store.Get("white.png"), NULL, &dstRect);

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
		//if ((int)currentTime % 100 == 0)
	//	{
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
	//	}
	}

	ecs.FreeKeyboard();

	return 0;
}