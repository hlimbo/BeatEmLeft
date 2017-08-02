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
#include "Utility/ImageStore.h"
#include "Utility/ImageMod.h"
#include "Utility/MapFileLoader.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	//setup texture file paths
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	string backgroundPath = mainPath + string("hills.png");
	string tilePath = mainPath + string("block.png");

	ImageStore store(render);
	store.Load("hills.png", backgroundPath);
	store.Load("block.png", tilePath);

	ECS ecs;

	int bgID = ecs.entitySystem.CreateEntity("Background");
	auto bgTransform = new Transform();
	bgTransform->position = Vect2(0.0f, 0.0f);
	auto bgSprite = new Sprite(store.Get("hills.png"));
	ecs.transforms.AddComponent(bgID, bgTransform);
	ecs.sprites.AddComponent(bgID, bgSprite);

	//initialize systems
	RenderSystem renderSys(&ecs);
	renderSys.Init(SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Point mousePos{ 0,0 };
	//screen to game world coordinates as ints
	SDL_Point gameCoords{ 0,0 };

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
					printf("left mouse click (%d,%d)\n", mousePos.x, mousePos.y);
					printf("game world coords(%d, %d)\n", gameCoords.x, gameCoords.y);
					float tileWidth = 56;
					float tileHeight = 56;
					if (mousePos.x > 0 && mousePos.x + (int)tileWidth < SCREEN_WIDTH)
					{
						cout << "spawn tile!" << endl;
						int tileID = ecs.entitySystem.CreateEntity("Tile");
						Vect2 tilePosition;
						tilePosition.x = (float)gameCoords.x;
						tilePosition.y = (float)gameCoords.y;
						ecs.transforms.AddComponent(tileID, new Transform(tilePosition));
						
						Sprite* sprite = new Sprite(store.Get("block.png"));
						sprite->width = tileWidth;
						sprite->height = tileHeight;
						ecs.sprites.AddComponent(tileID, sprite);
					}
				
				}
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				mousePos.x = event.button.x;
				mousePos.y = event.button.y;
				gameCoords.x = renderSys.camera.x + mousePos.x;
				gameCoords.y = renderSys.camera.y + mousePos.y;
			}
		}
		//pixels per frame
		int camSpeed = 2;
		if (mousePos.x + 5 > SCREEN_WIDTH)
		{
			int level_width = 800;
			if (renderSys.camera.x + renderSys.camera.w < level_width)
			{
				renderSys.camera.x += camSpeed;
			}
		}

		if (mousePos.x < 5)
		{
			if (renderSys.camera.x > 0)
				renderSys.camera.x -= camSpeed;
		}

		if (mousePos.y + 5 > SCREEN_HEIGHT)
		{
			int level_height = 600;
			if (renderSys.camera.y + renderSys.camera.h < level_height)
				renderSys.camera.y += camSpeed;
		}

		if (mousePos.y < 5)
		{
			if (renderSys.camera.y > 0)
				renderSys.camera.y -= camSpeed;
		}

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
		std::string title("Beat Em Left");
		title += std::string(" | FPS: ") + std::to_string(observedFPS);
		SDL_SetWindowTitle(core.getWindow(), title.c_str());
	
	}

	return 0;
}