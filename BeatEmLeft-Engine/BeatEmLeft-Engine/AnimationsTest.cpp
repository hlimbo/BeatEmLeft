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
	string playerPath = mainPath + string("blue.png");

	TextureStore store(render);
	store.Load("Background.png", backgroundPath);
	store.Load("block.png", tilePath);
	store.Load("adv_idle.png", idlePath);
	store.Load("adv_walk.png", walkPath);
	store.Load("adv_jump.png", jumpPath);
	store.Load("blue.png", playerPath);

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
			if (map.contents[r][c] == 1)
			{
				int tileID = ecs.entitySystem.CreateEntity("Tile");
				//construct list of tileCoordinates
				Vect2 tilePosition((float)c * (float)(tileWidth),
					(float)r * (float)(tileHeight));
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

	int playerID = ecs.entitySystem.CreateEntity("Player");
	
	auto playerTransform = new Transform();
	playerTransform->position = Vect2(20.0f, 0.0f);

	//Sprite
	auto playerSprite = new Sprite(store.Get("blue.png"));
	playerSprite->width = 20;
	playerSprite->height = 80;
	//temp variables
	float offSetLeft = 0.0f;//7.0f;
	float offSetTop = 0.0f;//1.0f;
	playerSprite->position.x = playerTransform->position.x + offSetLeft;
	playerSprite->position.y = playerTransform->position.y + offSetTop;

	//SpriteSheets / Animations
	auto playerAnimation = new Animation();
	playerAnimation->Add("idle", new SpriteSheet(store.Get("adv_idle.png"), 32, 64));
	playerAnimation->Add("walk", new SpriteSheet(store.Get("adv_walk.png"), 32, 64));
	playerAnimation->Add("jump", new SpriteSheet(store.Get("adv_jump.png"), 32, 64));

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
	playerKinematic->jumpSpeed = 110.6f;    
	playerKinematic->minJumpSpeed = 110.6f;
	playerKinematic->maxJumpSpeed = 550.5f;
	playerKinematic->jumpFactor = 10.5f;
	playerKinematic->currentJumpTime = 0.0f;
	playerKinematic->maxJumpTime = 0.65f;

	auto playerBox = new BoxCollider();
	playerBox->position = playerTransform->position;
	//playerBox->position.x += offSetLeft;
	//playerBox->position.y += offSetTop;
	playerBox->width = 20;
	playerBox->height = 80;

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

			keyboard->HandleInput(event);
		}

		keyboard->UpdateKeyStates(observedDeltaTime);

		movementSys.UpdateKinematics(deltaTime);
		movementSys.CheckForCollisions(observedDeltaTime);

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
	}

	ecs.FreeKeyboard();

	return 0;
}