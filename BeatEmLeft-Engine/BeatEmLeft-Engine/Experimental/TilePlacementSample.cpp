//use 800x600 pixel width and height

#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>

#include <Vect2.h>

#include "../Systems/RenderSystem.h"
#include "../Systems/ComponentManager.h"
#include "../Systems/EntitySystem.h"
#include "../Components/Sprite.h"
#include "../Components/Transform.h"
#include "../Components/BoxCollider.h"
#include "../Components/SpriteSheet.h"
#include "../Components/Animation.h"
#include "../Components/SlopeCollider.h"
#include "../Systems/ECS.h"
#include "../Core.h"
#include "../Systems/MovementSystem.h"

#include "../Input/KeyboardController.h"
#include "../Input/GameController.h"
#include "../Utility/TextureLoader.h"
#include "../Utility/TextureStore.h"
#include "../Utility/ImageStore.h"
#include "../Utility/ImageMod.h"
#include "../Utility/MapFileLoader.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace std;

//Prototyping phase: The following functions I will consider to relocate to another .cpp or .h file
void destroyTiles(ECS* ecs)
{
	int deleteCount = 0;
	if (ecs->entitySystem.ContainsEntityType("Tile"))
	{
		vector<int> ids = ecs->entitySystem.GetIDs("Tile");
		for (vector<int>::iterator it = ids.begin();it != ids.end();++it)
		{
			ecs->transforms.DeleteComponent(*it);
			ecs->sprites.DeleteComponent(*it);
			ecs->entitySystem.RemoveEntity("Tile", *it);
			++deleteCount;
		}
	}
	//debug
	printf("Number of tiles deleted: %d\n", deleteCount);
}

void constructTile(ECS* ecs, ImageStore* store, int row, int col, int tileWidth, int tileHeight, int x_offset, int y_offset)
{
	int tileID = ecs->entitySystem.CreateEntity("Tile");
	Vect2 tilePosition;
	tilePosition.x = col * (float)tileWidth + x_offset;
	tilePosition.y = row * (float)tileHeight + y_offset;
	ecs->transforms.AddComponent(tileID, new Transform(tilePosition));
	//may make ImageStore a singleton since I only need exactly one imagestore instance in my code.
	Sprite* sprite = new Sprite(store->Get("box.png"));
	sprite->width = (int)tileWidth;
	sprite->height = (int)tileHeight;
	ecs->sprites.AddComponent(tileID, sprite);
}

//returns a new pre-rendered grid for the tile map editor
SDL_Texture* preRenderGrid(Core& core,int level_width,int level_height,int tile_width,int tile_height, SDL_Texture* oldGrid = NULL)
{
	if(oldGrid != NULL)
		SDL_DestroyTexture(oldGrid);

	Uint32 pixelFormat = SDL_GetWindowPixelFormat(core.getWindow());
	SDL_Renderer* render = core.getRenderer();
	SDL_Texture* newGrid = SDL_CreateTexture(render, pixelFormat, SDL_TEXTUREACCESS_TARGET, level_width, level_height);
	SDL_SetTextureBlendMode(newGrid, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(render, newGrid);

	//transparent background
	SDL_SetRenderDrawColor(render, 255, 255, 255, 0);
	SDL_RenderClear(render);
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);

	int tiles_per_col = level_height / tile_height;
	int tiles_per_row = level_width / tile_width;
	//center the tiles about the background
	int x_offset = 0;//tile_width / 4;
	int y_offset = tile_height / 2;

	for (int r = 0;r < tiles_per_col;++r)
	{
		for (int c = 0;c < tiles_per_row;++c)
		{
			SDL_Rect gridBox;
			gridBox.x = c * tile_width + x_offset;
			gridBox.y = r * tile_height + y_offset;
			gridBox.w = tile_width;
			gridBox.h = tile_height;
			SDL_RenderDrawRect(render, &gridBox);
		}
	}

	SDL_RenderPresent(render);
	//set the render target back to the window's screen.
	SDL_SetRenderTarget(render, NULL);

	return newGrid;
}

//returns a new pre-rendered SDL_Texture
//returns a whiteBox with initialized values
SDL_Texture* preRenderWhiteBox(Core& core,int tile_width,int tile_height,SDL_Rect* whiteBox,SDL_Texture* oldBox = NULL)
{
	if(oldBox != NULL)
		SDL_DestroyTexture(oldBox);

	whiteBox->w = tile_width;
	whiteBox->h = tile_height;
	whiteBox->x = 0;
	whiteBox->y = 0;

	Uint32 pixelFormat = SDL_GetWindowPixelFormat(core.getWindow());
	SDL_Renderer* render = core.getRenderer();
	SDL_Texture* white = SDL_CreateTexture(render, pixelFormat, SDL_TEXTUREACCESS_TARGET, whiteBox->w, whiteBox->h);
	SDL_SetRenderTarget(render, white);
	SDL_SetTextureBlendMode(white, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
	//SDL_RenderFillRect(render, whiteBox);
	SDL_RenderDrawRect(render, whiteBox);
	SDL_RenderPresent(render);
	SDL_SetRenderTarget(render, NULL);

	return white;
}

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	//setup texture file paths
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	//string backgroundPath = mainPath + string("hills.png");
	string tilePath = mainPath + string("box.png");

	//should make into a singleton
	ImageStore store(render);
	store.Load("box.png", tilePath);


	string backgroundName;
	string backgroundPath;

	string levelName;
	string levelPath;
	int level_width, level_height;
	int tileWidth, tileHeight;
	cout << "Welcome to the Level Map Editor Demo!" << endl;
	cout << "Enter in name of file to save as: ";
	getline(cin, levelName);
	levelPath = mainPath + levelName;
	
	cout << "Enter in background file to use: ";
	getline(cin, backgroundName);
	backgroundPath = mainPath + backgroundName;
	store.Load(backgroundName, backgroundPath);
	
	//should probably turn into a helper function
	SDL_QueryTexture(store.Get(backgroundName)->texture, NULL, NULL, &level_width, &level_height);

	cout << "Enter in tileWidth: ";
	cin >> tileWidth;
	cout << "Enter in tileHeight: ";
	cin >> tileHeight;
	fflush(stdin);

	//draw a grid - pre render a static image.
	int tiles_per_row = level_width / (int)tileWidth;
	int tiles_per_col = level_height / (int)tileHeight;

	//attempt to center grid about the background
	int x_offset = 0;//(int)tileWidth / 4;
	int y_offset = (int)tileHeight / 2;

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

	//draw grid on some texture
	SDL_Texture* grid = preRenderGrid(core, level_width, level_height, tileWidth, tileHeight);
	//create a white translucent box to show what tile is being placed.
	SDL_Rect whiteBox;
	SDL_Texture* white = preRenderWhiteBox(core, tileWidth, tileHeight,&whiteBox);

	ECS ecs;

	int bgID = ecs.entitySystem.CreateEntity("Background");
	auto bgTransform = new Transform();
	bgTransform->position = Vect2(0.0f, 0.0f);
	auto bgSprite = new Sprite(store.Get(backgroundName));
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
						//write image file location
						file << backgroundPath << endl;
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
					if (levelName == "\n" || levelName == "")
						getline(cin, levelName);

					levelPath = mainPath + levelName;
					ifstream inputFile(levelPath,ios_base::in);
					if (inputFile.good())
					{
						cout << "loading map: " << levelName << endl;

						//read image filepath
						getline(inputFile, backgroundPath);
						//split string on last occurence of \ for windows, / for linux
						size_t lastIndex = backgroundPath.find_last_of("/\\");
						backgroundName = backgroundPath.substr(lastIndex + 1);
						
						cout << backgroundPath << endl;
						cout <<"background filename: " <<  backgroundName << endl;

						//returns the source image if background image is already loaded in the image store
						Image* src_image = store.Load(backgroundName, backgroundPath);
						Sprite* bg_sprite = ecs.sprites.GetComponent(bgID);
						if (src_image != nullptr && bg_sprite != nullptr)
						{
							bg_sprite->SetTextureAttributes(src_image->texture);
						}

						//read level_dimensions
						inputFile >> level_width >> level_height;
						//read tile_dimensions
						inputFile >> tileWidth >> tileHeight;

						//destroy all tiles from previous loaded map
						destroyTiles(&ecs);

						//free the tiles
						for (int i = 0;i < tiles_per_col;++i)
							free(tiles[i]);
						free(tiles);

						// have to recalculate the x_offset and y_offset;
						x_offset = 0;
						y_offset = (int)tileHeight / 2;
						//have to recalculate tiles_per_row and tiles_per_col here
						tiles_per_row = level_width / (int)tileWidth;
						tiles_per_col = level_height / (int)tileHeight;

						//reallocate bigger sized array here
						tiles = (int**)malloc(sizeof(int*) * tiles_per_col);
						for (int i = 0;i < tiles_per_col;++i)
						{
							tiles[i] = (int*)malloc(sizeof(int) * tiles_per_row);
						}

						//have to reconstruct grid here
						grid = preRenderGrid(core, level_width, level_height, tileWidth, tileHeight, grid);
						//resize whitebox here
						whiteBox.w = tileWidth;
						whiteBox.h = tileHeight;	

						for (int r = 0;r < tiles_per_col;++r)
						{
							for (int c = 0;c < tiles_per_row;++c)
							{
								inputFile >> tiles[r][c];
								//reconstruct the tiles from the data
								if (tiles[r][c] == 1)
								{
									constructTile(&ecs, &store, r, c, tileWidth, tileHeight, x_offset, y_offset);
								}

							}
						}

						cout << "Load complete!" << endl;
					}
					else
					{
						cout << "file could not be opened" << endl;
					}

					inputFile.close();
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
						constructTile(&ecs, &store, r, c, tileWidth, tileHeight, x_offset, y_offset);
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
									ecs.transforms.DeleteComponent(*it);
									ecs.sprites.DeleteComponent(*it);
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