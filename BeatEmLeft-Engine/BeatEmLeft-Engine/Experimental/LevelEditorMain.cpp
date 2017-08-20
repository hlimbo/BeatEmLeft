#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../MasterHeader.h"
using namespace std;

//helper function for setting the color
void SetDrawColor(SDL_Renderer* render, const SDL_Color& color)
{
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
}

//temp global variables
float vertSliderValue = 0.0f;

//note: might want to not use function pointers to store GUI elements since some elements
//such as sliders and toggles need to retain their state.
//todo: find a way to remove TTF_Font* parameter pass to all GUI related functions that need to display text
//e.g. GUI::Label() and GUI::TextField()
bool WindowFunction(int ui_id,const SDL_Rect* relativePos,TTF_Font* font)
{
	SDL_Rect buttonRect{ relativePos->x + 100 - 75 / 2,relativePos->y + 100 - 20 / 2,75,20 };
	SDL_Color blue2{ 0,0,255,255 };
	int buttonID = __LINE__;
	if (GUI::Button(buttonID, &buttonRect, blue2, "OK", font))
	{
		printf("Window ID: %d\nButton ID: %d\n", ui_id, buttonID);
		puts("Button pressed from window");
	}

	SDL_Rect vertSliderRect{relativePos->x + 10,relativePos->y + 10,20,150 };
	int sliderID = __LINE__;
	vertSliderValue = GUI::VerticalSlider(sliderID, &vertSliderRect, vertSliderValue, SDL_Color{ 255,0,0,255 });

	if (GUI::ui_global_state.keyboardFocusID == sliderID)
		puts("keyboard has focus on slider");
	else if (GUI::ui_global_state.keyboardFocusID != 0)
		puts("keyboard has focus on another thing");

	//temp
	return GUI::ui_global_state.keyboardFocusID != ui_id;
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
	SDL_Color blue{53,167,255,255};
	SDL_Color green{ 56,230,140,255 };

	//positions of each panel
	SDL_Rect toolbarRect;
	toolbarRect.x = 0;
	toolbarRect.y = 0;
	toolbarRect.w = SCREEN_WIDTH;
	toolbarRect.h = 20;
	SDL_Rect levelWindowRect{ 0,20,800,600 };
	SDL_Rect sidePanelRect{ 800,20,400,600 };

	SDL_Rect tilePreviewRect{ 825,50,150,150 };
	SDL_Rect tileSetRect{ 805,250, 190, 360 };

	SDL_Rect guiWindowRect{ 100,100,200,200 };

	bool isToggled = true;
	SDL_Rect toggleBounds{ 100,50, 25,25 };

	float sliderValue2 = 0.0f;
	SDL_Rect sliderRect2{ 400,50,20,200 };

	//---------------- Game Loop ------------------//

	GUI::Init(render);
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
		
		SetDrawColor(render, yellow);
		SDL_RenderFillRect(render, &tilePreviewRect);

		SetDrawColor(render, green);
		SDL_RenderFillRect(render, &tileSetRect);

		isToggled = GUI::Toggle(__LINE__, &toggleBounds, isToggled, red);
		SDL_Point labelPos{ 135,50 };
		GUI::Label(__LINE__, &labelPos, font, "Enable/Disable Window", red);

		if (isToggled)
		{
			//Task 2: draw gui window
			guiWindowRect = GUI::Window(__LINE__, &guiWindowRect, font, WindowFunction);
			SDL_SetRenderDrawColor(render, blue.r, blue.g, blue.b, blue.a);
			SDL_RenderFillRect(render, &toolbarRect);
		}

		sliderValue2 = GUI::VerticalSlider(__LINE__, &sliderRect2, sliderValue2, green);

		SDL_RenderPresent(render);
		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_RenderClear(render);

		GameLoop::UpdateFPS();
		GameLoop::CapFramerate(core.getTargetDeltaTime());
		float currentTime = GameLoop::UpdateCurrentTime();
		GameLoop::DisplayFPS(core.getWindow(), 500.0f);

		GUI::SetTimeAndOldMousePos(currentTime);
	}
	

	TTF_CloseFont(font);
	return 0;
}