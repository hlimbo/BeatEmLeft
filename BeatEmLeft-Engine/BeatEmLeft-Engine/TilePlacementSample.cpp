#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>

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
	string tilePath = mainPath + string("box.png");

	ImageStore store(render);
	store.Load("hills.png", backgroundPath);
	store.Load("box.png", tilePath);

	string levelName;
	string levelPath;
	int level_width, level_height;
	int tileWidth, tileHeight;
	cout << "Welcome to the Level Map Editor Demo!" << endl;
	cout << "Enter in name of file to save as: ";
	getline(cin, levelName);
	levelPath = mainPath + levelName;
	cout << "Enter in level_width: ";
	cin >> level_width;
	cout << "Enter in level_height: ";
	cin >> level_height;
	cout << "Enter in tileWidth: ";
	cin >> tileWidth;
	cout << "Enter in tileHeight: ";
	cin >> tileHeight;
	fflush(stdin);

	//draw a grid - pre render a static image.
	int tiles_per_row = level_width / (int)tileWidth;
	int tiles_per_col = level_height / (int)tileHeight;

	//temp hard coded values to center grid about the background image
	const int x_offset = 17 / 2;
	const int y_offset = 41 / 2;

	//c-style dynamic allocation
	int** tiles = (int**)malloc(sizeof(int*) * tiles_per_col);
	for (int i = 0;i < tiles_per_col;++i)
	{
		tiles[i] = (int*)malloc(sizeof(int) * tiles_per_row);
	}

	for (int r = 0;r < tiles_per_col;++r)
	{
		for (int c = 0;c < tiles_per_row;++c)
		{
			tiles[r][c] = 0;
		}
	}

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

	//create a white translucent box to show what tile is being placed.
	SDL_Rect whiteBox;
	whiteBox.w = (int)tileWidth;
	whiteBox.h = (int)tileHeight;
	whiteBox.x = 0;
	whiteBox.y = 0;

	SDL_Texture* white = SDL_CreateTexture(render,SDL_GetWindowPixelFormat(core.getWindow()), SDL_TEXTUREACCESS_TARGET, whiteBox.w, whiteBox.h);
	SDL_SetRenderTarget(render, white);
	SDL_SetTextureBlendMode(white, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255 / 2);
	SDL_RenderFillRect(render, &whiteBox);
	SDL_RenderPresent(render);
	SDL_SetRenderTarget(render, NULL);

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

			if (event.type == SDL_KEYDOWN)
			{
				//save map to .txt file
				if (event.key.keysym.sym == SDLK_s)
				{
					cout << "Saving file.... " << levelName << endl;
					//file stuff
					ofstream file;
					file.open(levelPath, ifstream::out);
					if (file.good())
					{
						//write level dimensions
						file << level_width << " " << level_height << endl;
						//write tile dimensions
						file << tileWidth << " " << tileHeight << endl;

						//write level map layout
						for (int r = 0;r < tiles_per_col;++r)
						{
							for (int c = 0;c < tiles_per_row;++c)
							{
								file << tiles[r][c] << " ";
							}
							file << endl;
						}

						file.close();
						cout << "Save complete in " << levelPath << endl;
					}
					else
					{
						cout << "file is bad: " << levelPath << endl;
					}
				}

				//load map
				if (event.key.keysym.sym == SDLK_l)
				{
					string levelName;
					string levelPath;
					cout << "Level to Load: ";
					getline(cin, levelName);
					levelPath = mainPath + levelName;
					ifstream inputFile(levelPath,ios_base::in);
					if (inputFile.good())
					{
						cout << "loading map: " << levelName << endl;

						//read level_dimensions
						inputFile >> level_width >> level_height;
						//read tile_dimensions
						inputFile >> tileWidth >> tileHeight;

						//free the tiles
						for (int i = 0;i < tiles_per_col;++i)
							free(tiles[i]);
						free(tiles);

						//have to recalculate tiles_per_row and tiles_per_col here
						tiles_per_row = level_width / (int)tileWidth;
						tiles_per_col = level_height / (int)tileHeight;

						//reallocate bigger sized array here
						tiles = (int**)malloc(sizeof(int*) * tiles_per_col);
						for (int i = 0;i < tiles_per_col;++i)
						{
							tiles[i] = (int*)malloc(sizeof(int) * tiles_per_row);
						}

						for (int r = 0;r < tiles_per_col;++r)
						{
							for (int c = 0;c < tiles_per_row;++c)
							{
								tiles[r][c] = 0;
							}
						}

						//have to reconstruct grid here
						SDL_DestroyTexture(grid);
						grid = SDL_CreateTexture(render, SDL_GetWindowPixelFormat(core.getWindow()), SDL_TEXTUREACCESS_TARGET, level_width, level_height);
						SDL_SetTextureBlendMode(grid, SDL_BlendMode::SDL_BLENDMODE_BLEND);
						SDL_SetRenderTarget(render, grid);
						//transparent background
						SDL_SetRenderDrawColor(render, 255, 255, 255, 0);
						SDL_RenderClear(render);
						//black
						SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
						for (int ry = 0;ry < tiles_per_col;++ry)
						{
							for (int cx = 0;cx < tiles_per_row;++cx)
							{
								SDL_Rect gridBox;
								gridBox.x = cx * (int)tileWidth + x_offset;
								gridBox.y = ry * (int)tileHeight + y_offset;
								gridBox.w = (int)tileWidth;
								gridBox.h = (int)tileHeight;
								SDL_RenderDrawRect(render, &gridBox);
							}
						}

						SDL_RenderPresent(render);
						//set the render target back to the window's screen.
						SDL_SetRenderTarget(render, NULL);


						//resize whitebox here
						whiteBox.w = (int)tileWidth;
						whiteBox.h = (int)tileHeight;
						whiteBox.x = 0;
						whiteBox.y = 0;

						SDL_DestroyTexture(white);
						white = SDL_CreateTexture(render, SDL_GetWindowPixelFormat(core.getWindow()), SDL_TEXTUREACCESS_TARGET, whiteBox.w, whiteBox.h);
						SDL_SetRenderTarget(render, white);
						SDL_SetTextureBlendMode(white, SDL_BLENDMODE_BLEND);
						SDL_SetRenderDrawColor(render, 255, 255, 255, 255 / 2);
						SDL_RenderFillRect(render, &whiteBox);
						SDL_RenderPresent(render);
						SDL_SetRenderTarget(render, NULL);

						for (int r = 0;r < tiles_per_col;++r)
						{
							for (int c = 0;c < tiles_per_row;++c)
							{
								int tileNum;
								inputFile >> tileNum;
								//remove tiles that weren't loaded in from the .txt file
								if (tileNum == 0 && tiles[r][c] == 1)
								{
									//slow operation and is temporary as I'm trying to move towards using json files to store more information
									//about the level's data such as id
									vector<int> ids = ecs.entitySystem.GetIDs();
									for (auto it = ids.begin();it != ids.end();++it)
									{
										Transform* transform = ecs.transforms.GetComponent(*it);
										if (transform != nullptr)
										{
											int ry = (int)transform->position.y / (int)tileHeight;
											int cx = (int)transform->position.x / (int)tileWidth;
											if (r == ry && c == cx)
											{
												ecs.entitySystem.RemoveEntity("Tile", *it);
												Transform* rTrans = ecs.transforms.RemoveComponent(*it);
												Sprite* rSprite = ecs.sprites.RemoveComponent(*it);
												if (rTrans != nullptr)
												{
													delete rTrans;
													rTrans = nullptr;
												}

												if (rSprite != nullptr)
												{
													delete rSprite;
													rSprite = nullptr;
												}
											}
										}
									}
								}
								//reconstruct the tiles from the data
								else if (tileNum == 1 && tiles[r][c] == 0)
								{
									int tileID = ecs.entitySystem.CreateEntity("Tile");
									Vect2 tilePosition;
									tilePosition.x = c * (float)tileWidth + x_offset;
									tilePosition.y = r * (float)tileHeight + y_offset;
									ecs.transforms.AddComponent(tileID, new Transform(tilePosition));
									Sprite* sprite = new Sprite(store.Get("box.png"));
									sprite->width = (int)tileWidth;
									sprite->height = (int)tileHeight;
									ecs.sprites.AddComponent(tileID, sprite);

								}

								tiles[r][c] = tileNum;
							}
						}

						inputFile.close();
						cout << "Load complete!" << endl;
					}
					else
					{
						cout << "file could not be opened" << endl;
					}
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				//add a tile
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					printf("left mouse click (%d,%d)\n", mousePos.x, mousePos.y);
					printf("game world coords(%d, %d)\n", gameCoords.x, gameCoords.y);

					//clamp tile position to grid.
					int r = gameCoords.y / (int)tileHeight;
					int c = gameCoords.x / (int)tileWidth;
					if (r < tiles_per_col && c < tiles_per_row && r >= 0 && c >= 0 && tiles[r][c] == 0)
					{
						cout << "spawn tile" << endl;
						tiles[r][c] = 1;
						Vect2 tilePosition;
						tilePosition.x = c * (float)tileWidth + x_offset;
						tilePosition.y = r * (float)tileHeight + y_offset;
						int tileID = ecs.entitySystem.CreateEntity("Tile");
						ecs.transforms.AddComponent(tileID, new Transform(tilePosition));

						Sprite* sprite = new Sprite(store.Get("box.png"));
						sprite->width = (int)tileWidth;
						sprite->height = (int)tileHeight;
						ecs.sprites.AddComponent(tileID, sprite);
					}
				
				}

				//remove tile
				if (event.button.button == SDL_BUTTON_RIGHT)
				{
					int r = gameCoords.y / (int)tileHeight;
					int c = gameCoords.x / (int)tileWidth;

					if (r < tiles_per_col && c < tiles_per_row && r >= 0 && c >= 0 && tiles[r][c] == 1)
					{
						printf("remove tile at: (%d,%d)\n", gameCoords.x, gameCoords.y);
						tiles[r][c] = 0;
						vector<int> ids = ecs.entitySystem.GetIDs("Tile");
						for (auto it = ids.begin();it != ids.end();++it)
						{
							Transform* transform = ecs.transforms.GetComponent(*it);
							if (transform != nullptr)
							{
								int ry = (int)transform->position.y / (int)tileHeight;
								int cx = (int)transform->position.x / (int)tileWidth;
								if (r == ry && c == cx)
								{
									ecs.entitySystem.RemoveEntity("Tile", *it);
									Transform* rTrans = ecs.transforms.RemoveComponent(*it);
									Sprite* rSprite = ecs.sprites.RemoveComponent(*it);
									if (rTrans != nullptr)
									{
										delete rTrans;
										rTrans = nullptr;
									}

									if (rSprite != nullptr)
									{
										delete rSprite;
										rSprite = nullptr;
									}
								}
							}
						}
					}
				}
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				mousePos.x = event.button.x;
				mousePos.y = event.button.y;
				gameCoords.x = renderSys.camera.x + mousePos.x;
				gameCoords.y = renderSys.camera.y + mousePos.y;

				whiteBox.x = ((gameCoords.x / (int)tileWidth) * (int)tileWidth + x_offset) - renderSys.camera.x;
				whiteBox.y = ((gameCoords.y / (int)tileHeight) * (int)tileHeight + y_offset) - renderSys.camera.y;
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
		SDL_RenderCopy(render, white, NULL, &whiteBox);
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
	SDL_DestroyTexture(white);

	//c-style deallocation
	for (int i = 0;i < tiles_per_col;++i)
		free(tiles[i]);
	free(tiles);

	return 0;
}