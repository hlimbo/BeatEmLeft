#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <Vect2.h>

#include "../Core.h"
#include "../Systems/ECS.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/MovementSystem.h"

#include "../Systems/ComponentManager.h"
#include "../Systems/EntitySystem.h"
#include "../Components/Sprite.h"
#include "../Components/Transform.h"
#include "../Components/BoxCollider.h"
#include "../Components/SpriteSheet.h"

#include "../Input/KeyboardController.h"
#include "../Utility/TextureStore.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	ECS ecs;
	ecs.InitKeyboard();

	//setup texture file paths
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	string playerPath = mainPath + string("blue.png");
	string backgroundPath = mainPath + string("Background.png");
	string redPath = mainPath + string("redblock.png");
	string blockPath = mainPath + string("block.png");

	TextureStore store(render);
	store.Load("Background.png", backgroundPath);
	store.Load("block.png", blockPath);
	store.Load("blue.png", playerPath);
	store.Load("redblock.png", redPath);

	int bgID = ecs.entitySystem.CreateEntity("Background");
	auto bgTransform = new Transform();
	bgTransform->position = Vect2(0.0f, 0.0f);
	auto bgSprite = new Sprite(store.Get("Background.png"));
	ecs.transforms.AddComponent(bgID, bgTransform);
	ecs.sprites.AddComponent(bgID, bgSprite);

	int blockID = ecs.entitySystem.CreateEntity("Tile");
	auto blockTransform = new Transform(Vect2(100.0f,0.0f));
	auto blockSprite = new Sprite(store.Get("redblock.png"));
	blockSprite->width = 40;
	blockSprite->height = 40;
	auto blockCollider = new BoxCollider();
	blockCollider->position = blockTransform->position;
	blockCollider->width = 40.0f;
	blockCollider->height = 40.0f;

	ecs.transforms.AddComponent(blockID, blockTransform);
	ecs.sprites.AddComponent(blockID, blockSprite);
	ecs.boxColliders.AddComponent(blockID, blockCollider);
	

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

	//gravity
	playerKinematic->gravity = 0.0f;
	playerKinematic->minGravity = 0.0f;
	playerKinematic->maxGravity = 0.0f;
	playerKinematic->gravityFactor = 0.0f;

	//jumping
	playerKinematic->jumpSpeed = 85.6f;
	playerKinematic->minJumpSpeed = 85.6f;
	playerKinematic->maxJumpSpeed = 495.0f;
	playerKinematic->jumpFactor = 10.5f;
	playerKinematic->currentJumpTime = 0.0f;
	playerKinematic->maxJumpTime = 0.65f;

	auto playerBox = new BoxCollider();
	playerBox->position = playerTransform->position;
	playerBox->width = (float)playerSprite->width;
	playerBox->height = (float)playerSprite->height;

	ecs.transforms.AddComponent(playerID, playerTransform);
	ecs.sprites.AddComponent(playerID, playerSprite);
	ecs.kinematics.AddComponent(playerID, playerKinematic);
	ecs.boxColliders.AddComponent(playerID, playerBox);

	KeyboardController* keyboard = ecs.RegisterKeyboard(playerID);

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
					printf("left mouse click (%d,%d)\n", event.button.x, event.button.y);
				}
			}

			keyboard->HandleInput(event);
		}
		keyboard->UpdateKeyStates(observedDeltaTime);
		//keyboard->PrintKeyStatuses();

		movementSys.UpdateKinematics(deltaTime);
		movementSys.CheckForCollisions(observedDeltaTime);

		movementSys.UpdatePositions(deltaTime);
		movementSys.CorrectCollisionOverlaps(observedDeltaTime);

		//check for rectangle overlap
		Vect2 playerPos = playerTransform->position;
		Vect2 boxPos = blockTransform->position;

		//if (playerPos.x < boxPos.x + blockCollider->width &&
		//	playerPos.x + playerBox->width > boxPos.x)
		//{
		//	puts("detected x-axis overlap");
		//}

		//if (playerPos.y < boxPos.y + blockCollider->height &&
		//	playerPos.y + playerBox->height > boxPos.y)
		//{
		//	puts("detected y-axis overlap");
		//}

		//if (playerPos.x < boxPos.x + blockCollider->width &&
		//	playerPos.x + playerBox->width > boxPos.x  &&
		//	playerPos.y < boxPos.y + blockCollider->height &&
		//	playerPos.y + playerBox->height > boxPos.y)
		//{
		//	Vect2 normal = VectMath::Normalize(playerKinematic->velocity);

		//	Vect2 pen = playerPos - boxPos;

		//	if (normal.x > 0.0f)
		//	{
		//		playerTransform->position.x += (pen.x + playerBox->width) * -1.0f * normal.x;
		//	}
		//	else if (normal.y < 0.0f)
		//	{

		//	}
		//}


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