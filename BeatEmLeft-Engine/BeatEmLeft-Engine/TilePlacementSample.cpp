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
	
	//draw a grid - pre render a static image.
	const float tileWidth = 56;
	const float tileHeight = 56;
	const int level_width = 800;
	const int level_height = 600;
	const int tiles_per_row = level_width / (int)tileWidth;
	const int tiles_per_col = level_height / (int)tileHeight;
	const int x_offset = 17 / 2;
	const int y_offset = 41 / 2;

	SDL_Texture* grid = SDL_CreateTexture(render, SDL_GetWindowPixelFormat(core.getWindow()), SDL_TEXTUREACCESS_TARGET, level_width, level_height);
	SDL_SetTextureBlendMode(grid, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(render, grid);
	//transparent background
	SDL_SetRenderDrawColor(render, 255, 255, 255, 0);
	SDL_RenderClear(render);
	//black
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	for (int r = 0;r < tiles_per_col;++r)
	{
		for (int c = 0;c < tiles_per_row;++c)
		{
			SDL_Rect gridBox;
			gridBox.x = c * (int)tileWidth + x_offset;
			gridBox.y = r * (int)tileHeight + y_offset;
			gridBox.w = (int)tileWidth;
			gridBox.h = (int)tileHeight;
			SDL_RenderDrawRect(render,&gridBox);
		}
	}

	SDL_RenderPresent(render);
	//set the render target back to the window's screen.
	SDL_SetRenderTarget(render, NULL);

	//setup texture file paths
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	string backgroundPath = mainPath + string("hills.png");
	string tilePath = mainPath + string("box.png");

	ImageStore store(render);
	store.Load("hills.png", backgroundPath);
	store.Load("box.png", tilePath);

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
				//add a tile
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					printf("left mouse click (%d,%d)\n", mousePos.x, mousePos.y);
					printf("game world coords(%d, %d)\n", gameCoords.x, gameCoords.y);
					//if (mousePos.x > 0 && mousePos.x + (int)tileWidth < SCREEN_WIDTH)
					//{
						int tileID = ecs.entitySystem.CreateEntity("Tile");
						Vect2 tilePosition;
					//	tilePosition.x = (float)gameCoords.x;
					//	tilePosition.y = (float)gameCoords.y;

						//clamp tile position to grid.
						int r = gameCoords.y / (int)tileHeight;
						int c = gameCoords.x / (int)tileWidth;
						tilePosition.x = c * tileWidth + x_offset;
						tilePosition.y = r * tileHeight + y_offset;

						ecs.transforms.AddComponent(tileID, new Transform(tilePosition));
						
						Sprite* sprite = new Sprite(store.Get("box.png"));
						sprite->width = (int)tileWidth;
						sprite->height = (int)tileHeight;
						ecs.sprites.AddComponent(tileID, sprite);
					//}
				
				}

				//TODO: remove at tile (create a simple data structure that tells us if a tile is occupying a space or not
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					//check if point overlaps in a rectangle!
					SDL_Point pixelCoords;
					pixelCoords.y = gameCoords.y / (int)tileHeight;
					pixelCoords.x = gameCoords.x / (int)tileWidth;
					pixelCoords.y = (pixelCoords.y * (int)tileHeight) + y_offset;
					pixelCoords.x = (pixelCoords.x * (int)tileWidth) + x_offset;

					//.....

					printf("remove tile at: (%d,%d)\n", gameCoords.x, gameCoords.y);
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
		SDL_RenderCopy(render, grid, &renderSys.camera, NULL);
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

	SDL_DestroyTexture(grid);

	return 0;
}