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
				auto transform =
					new Transform(Vect2((float)c * (float)(tileWidth + pad),
					(float)r * (float)(tileHeight + pad)));

				auto sprite = new Sprite(store.Get("block.png"));
				sprite->width = tileWidth;
				sprite->height = tileHeight;
			
				ecs.transforms.AddComponent(tileID, transform);
				ecs.sprites.AddComponent(tileID, sprite);
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
		movementSys.UpdatePositions(deltaTime);

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