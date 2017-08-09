#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "../MasterHeader.h"

using namespace std;

//returns a new pre-rendered SDL_Texture
//returns a whiteBox with initialized values
SDL_Texture* preRenderWhiteBox(Core& core, int tile_width, int tile_height, SDL_Rect* whiteBox, SDL_Texture* oldBox = NULL)
{
	if (oldBox != NULL)
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
	//might have found a bug where render draw color changes the default render target's color as well.
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
	//SDL_RenderFillRect(render, whiteBox);
	SDL_RenderDrawRect(render, whiteBox);
	SDL_RenderPresent(render);
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	SDL_SetRenderTarget(render, NULL);

	return white;
}

void constructTile(ECS* ecs,SpriteSheet* srcSheet,int sheetIndex, int row, int col, int tileWidth, int tileHeight)
{
	int tileID = ecs->entitySystem.CreateEntity("Tile");
	Vect2 tilePosition;
	tilePosition.x = col * (float)tileWidth;
	tilePosition.y = row * (float)tileHeight;
	ecs->transforms.AddComponent(tileID, new Transform(tilePosition));
	//example usage of constructing a sprite from a spritesheet.
	Sprite* sprite = new Sprite(srcSheet, sheetIndex);
	sprite->width = tileWidth;
	sprite->height = tileHeight;
	ecs->sprites.AddComponent(tileID, sprite);
}

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	ImageStore store(render);
	ECS ecs;

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	//cout << "Enter in tiles map image file to load: ";
	string tilesName("tiles-stones.png");
	string tilesPath;
	//getline(cin, tilesName);
	//fflush(stdin);
	tilesPath = mainPath + tilesName;
	Image* srcImg = store.Load(tilesName, tilesPath);

	//these are widths and heights of the image relative to the source's texture map
	int src_tile_w = 64;
	int src_tile_h = 64;
	//cout << "Enter in src_tile_w: ";
	//cin >> tileWidth;
	//cout << "Enter in src_tile_h: ";
	//cin >> tileHeight;

	SpriteSheet tileSheet(render, srcImg, src_tile_w, src_tile_h);
	cout << "frameCount: " << tileSheet.GetFrameCount() << endl;

	for (int i = 0;i < tileSheet.GetFrameCount();++i)
	{
		const SDL_Rect* frame = tileSheet.GetFrame(i);
		cout << "frame " << i << ": " << "(" << frame->x << ", " << frame->y << ")" << endl;
	}
	

	SDL_Rect whiteBox;
	float scale = 0.25f;
	int boxWidth = (int)(SCREEN_WIDTH * scale);
	int boxHeight = (int)(SCREEN_HEIGHT * scale);
	SDL_Texture* white = preRenderWhiteBox(core, boxWidth, boxHeight, &whiteBox);
	whiteBox.x = SCREEN_WIDTH / 2 - boxWidth / 2;
	whiteBox.y = SCREEN_HEIGHT / 2 - boxHeight / 2;

	//these are widths and heights of each tile relative to the game's window
	int tileWidth = 24, tileHeight = 24;

	//rescaling doesn't seem to work when drawing a texture on sdl with texture access target.
	SDL_Rect selectorBox;
	SDL_Texture* selector = preRenderWhiteBox(core, tileWidth, tileHeight, &selectorBox);
	selectorBox.x = 0;
	selectorBox.y = 0;

	SDL_Rect selectedRegion;
	selectedRegion.x = whiteBox.x;
	selectedRegion.y = whiteBox.y;
	selectedRegion.w = whiteBox.w;
	selectedRegion.h = whiteBox.h;

	SDL_Rect* selectableRects = (SDL_Rect*)malloc(sizeof(SDL_Rect) * tileSheet.GetFrameCount());
	for (int i = 0;i < tileSheet.GetFrameCount();++i)
	{
		selectableRects[i].x = i * tileWidth;
		selectableRects[i].y = SCREEN_HEIGHT - tileHeight;
		selectableRects[i].w = tileWidth;
		selectableRects[i].h = tileHeight;
	}

	RenderSystem renderSys(&ecs);
	renderSys.Init(SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Point mousePos{ 0,0 };
	int selectedFrame = 0;
	int actualWidth = 64;
	int actualHeight = 64;

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
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				int row = event.button.y / actualHeight;
				int col = event.button.x / actualWidth;
				constructTile(&ecs, &tileSheet,selectedFrame, row, col, actualWidth, actualHeight);
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				mousePos.x = event.button.x;
				mousePos.y = event.button.y;
			}
		}

		renderSys.Update(render);

		//render each tile from the tileSheet along the bottom side of the screen
		for (int i = 0;i < tileSheet.GetFrameCount();++i)
		{
			const SDL_Rect* frame = tileSheet.GetFrame(i);
			SDL_RenderCopy(render, tileSheet.texture, frame, &selectableRects[i]);
		}

		//check if mouse is on a specific tile
		for (int i = 0;i < tileSheet.GetFrameCount();++i)
		{
			if (SDL_PointInRect(&mousePos, &selectableRects[i]))
			{
				selectedFrame = i;
				selectorBox.x = selectableRects[i].x;
				selectorBox.y = selectableRects[i].y;
				SDL_RenderCopy(render, selector, NULL, &selectorBox);
			}
		}

		renderSys.Draw(render);

		//when a tile is selected,render the tile towards the center of the screen
		SDL_RenderCopy(render, tileSheet.texture, tileSheet.GetFrame(selectedFrame), &selectedRegion);
		//main box
		SDL_RenderCopy(render, white, NULL, &whiteBox);


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

	free(selectableRects);
	SDL_DestroyTexture(white);
	SDL_DestroyTexture(selector);
	return 0;
}
