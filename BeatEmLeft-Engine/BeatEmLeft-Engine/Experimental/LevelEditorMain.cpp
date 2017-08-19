#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../MasterHeader.h"
#include "../GameLoop.h"
using namespace std;

//helper function for setting the color
void SetDrawColor(SDL_Renderer* render, const SDL_Color& color)
{
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
}

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources/");

	//load in font
	string fontPath = mainPath + string("SourceCodePro-Black.ttf");
	int fontSize = 16;
	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);

	//load in sample tilesheet to use
	ImageStore imageStore(render);
	string imageFile("tiles-stones.png");
	string imagePath(mainPath + imageFile);
	Image* src = imageStore.Load(imageFile, imagePath);
	int frameWidth = 64, frameHeight = 64;
	SpriteSheet tileSheet(render, src, frameWidth, frameHeight);

	//color pallette
	SDL_Color yellow{ 255,231,76,255 };
	SDL_Color red{ 255,89,100,255 };
	SDL_Color white{ 255,255,255,255 };
	SDL_Color blue{53,167,255};
	SDL_Color green{ 56,230,140,255 };

	//positions of each panel
	SDL_Rect toolbarRect{ 0,0,SCREEN_WIDTH,20 };
	SDL_Rect levelWindowRect{ 0,20,800,600 };
	SDL_Rect sidePanelRect{ 800,20,400,600 };

	SDL_Rect tilePreviewRect{ 825,50,150,150 };
	SDL_Rect tileSetRect{ 805,250, 190, 360 };

	//---------------- Game Loop ------------------//

	GameLoop::InitTimer();
	bool running = true;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			GUI::ProcessEvent(&event);
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		/* GAME LOGIC FUNCTIONS GO HERE*/


		//Task : draw a mockup of where all the panels will go on the tile map editor window
		
		SetDrawColor(render, blue);
		SDL_RenderFillRect(render, &toolbarRect);
		
		SetDrawColor(render, white);
		SDL_RenderFillRect(render, &levelWindowRect);

		SetDrawColor(render, red);
		SDL_RenderFillRect(render, &sidePanelRect);

		SetDrawColor(render, blue);
		SDL_RenderFillRect(render, &toolbarRect);
		
		SetDrawColor(render, yellow);
		SDL_RenderFillRect(render, &tilePreviewRect);

		SetDrawColor(render, green);
		SDL_RenderFillRect(render, &tileSetRect);

		SDL_RenderPresent(render);
		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_RenderClear(render);

		GameLoop::UpdateFPS();
		GameLoop::CapFramerate(core.getTargetDeltaTime());
		GameLoop::UpdateCurrentTime();
		GameLoop::DisplayFPS(core.getWindow(), 500.0f);
	}
	

	TTF_CloseFont(font);
	return 0;
}