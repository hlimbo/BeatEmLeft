#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include "../MasterHeader.h"
using namespace std;

//color pallette
static SDL_Color yellow{ 255,231,76,255 };
static SDL_Color red{ 255,89,100,255 };
static SDL_Color white{ 255,255,255,255 };
static SDL_Color blue{ 53,167,255,255 };
static SDL_Color green{ 56,230,140,255 };

//helper function for setting the color
void SetDrawColor(SDL_Renderer* render, const SDL_Color& color)
{
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
}

//temp global variables to retain state for gui elements that will be contained within a window
//e.g. sliders and toggles need to retain state 
static float vertSliderValue = 0.0f;

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

	//temp ~ if window doesn't have exclusive focus with the mice (i.e. the mouse is pressing on other gui elements in the window)
	//the window should not be dragged
	return GUI::ui_global_state.keyboardFocusID != ui_id;
}

static string mapName;
static string levelWidth;
static string levelHeight;
static string tileWidth;
static string tileHeight;
static bool windowToggled = false;
//prototyped-canned code temporary
bool NewMapWindow(int ui_id, const SDL_Rect* relativePos, TTF_Font* font)
{
	int fontHeight = TTF_FontHeight(font);
	int titleYOffset = 10;
	int leftMargin = 10;

	int labelID = __LINE__;
	int titleWidth, titleHeight;
	TTF_SizeText(font, "New Map", &titleWidth, &titleHeight);
	//center title label along x-axis
	SDL_Point titlePos{ relativePos->x + (relativePos->w / 2 - titleWidth / 2), relativePos->y + titleYOffset};
	GUI::Label(labelID, &titlePos, font, "New Map", white);

	//map filename field
	int yOffset = (titlePos.y - relativePos->y) + titleHeight;
	SDL_Point labelPos{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
	GUI::Label(__LINE__, &labelPos, font, "Map Filename: ", white);

	int textFieldID = __LINE__;
	int widthOffset;
	TTF_SizeText(font, "Map Filename: ", &widthOffset, NULL);
	SDL_Rect textFieldPos{ relativePos->x + widthOffset + leftMargin,labelPos.y - 2,relativePos->w * 0.5f,fontHeight + 4};
	mapName = GUI::TextField(textFieldID, &textFieldPos, mapName, white, font);

	//level dimensions field
	yOffset = textFieldPos.y - relativePos->y + titleHeight;
	int levelDimensionsFieldID = __LINE__;
	SDL_Point label2Pos{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
	GUI::Label(levelDimensionsFieldID, &label2Pos, font, "Level WxH: ", white);
	TTF_SizeText(font, "Level WxH: ", &widthOffset, NULL);
	SDL_Rect textFieldPos2{ relativePos->x + leftMargin + widthOffset,label2Pos.y - 2,relativePos->w * 0.2f,fontHeight + 4 };
	levelWidth = GUI::TextField(__LINE__, &textFieldPos2, levelWidth, white, font);
	SDL_Point label3Pos{ relativePos->x + leftMargin + textFieldPos2.w + widthOffset,textFieldPos2.y };
	GUI::Label(__LINE__, &label3Pos, font, " x ", white);
	TTF_SizeText(font, " x ", &widthOffset, NULL);
	SDL_Rect textFieldPos3{ label3Pos.x + widthOffset, textFieldPos2.y,relativePos->w * 0.2f,fontHeight + 4 };
	levelHeight = GUI::TextField(__LINE__, &textFieldPos3, levelHeight, white, font);
	
	//tile dimensions field
	yOffset = textFieldPos3.y - relativePos->y + titleHeight;
	SDL_Point label4Pos{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
	GUI::Label(__LINE__, &label4Pos, font, "Tile WxH: ", white);
	TTF_SizeText(font, "Tile WxH: ", &widthOffset, NULL);
	SDL_Rect textFieldPos4{ relativePos->x + leftMargin + widthOffset,label4Pos.y - 2,relativePos->w * 0.2f,fontHeight + 4 };
	tileWidth = GUI::TextField(__LINE__, &textFieldPos4, tileWidth, white, font);
	SDL_Point label5Pos{ relativePos->x + leftMargin + textFieldPos4.w + widthOffset,textFieldPos4.y };
	GUI::Label(__LINE__, &label5Pos, font, " x ", white);
	TTF_SizeText(font, " x ", &widthOffset, NULL);
	SDL_Rect textFieldPos6{ label5Pos.x + widthOffset, textFieldPos4.y,relativePos->w * 0.2f,fontHeight + 4 };
	tileHeight = GUI::TextField(__LINE__, &textFieldPos6, tileHeight, white, font);


	int buttonWidth = relativePos->w * 0.2;
	int buttonHeight = relativePos->h * 0.2;
	SDL_Rect buttonRect{ relativePos->x + relativePos->w / 2 - buttonWidth / 2,relativePos->y + relativePos->h * 0.7,buttonWidth,buttonHeight};
	if (GUI::Button(__LINE__, &buttonRect, red, "OK", font))
	{
		//temporary functionality as this can be dangerous to do since creating a file that already exists
		//in the file system overrides that file!
		puts("Close this window and save the new file created");
		windowToggled = false;

		string mainPath(SDL_GetBasePath());
		mainPath += string("resources/");
		string filePath(mainPath + mapName);
		ofstream outputFile;
		outputFile.open(filePath, ifstream::out);

		if (outputFile.good())
		{
			//write map level name
			outputFile << mapName << endl;
			//write level dimensions
			outputFile << levelWidth << " " << levelHeight << endl;
			//write tile dimensions
			outputFile << tileWidth << " " << tileHeight << endl;
			outputFile.close();
		}
		else
		{
			printf("File is bad\n");
		}
	}
	

	return false;
}

bool ToolbarPrototype(int ui_id, const SDL_Rect* relativePos, TTF_Font* font)
{
	int newMapID = __LINE__;
	int saveMapID = __LINE__;
	int openMapID = __LINE__;
	int loadTileSetID = __LINE__;

	//construct rects for toolbar buttons
	int buttonWidth = 800 / 4;
	int buttonHeight = relativePos->h;
	int margin = 2;
	SDL_Rect buttonRects[4];
	for (int i = 0;i < 4;++i)
	{
		buttonRects[i].w = buttonWidth;
		buttonRects[i].h = buttonHeight;
		buttonRects[i].x = (i * (buttonWidth + margin)) + relativePos->x;
		buttonRects[i].y = relativePos->y;
	}

	if (GUI::Button(newMapID, &buttonRects[0], blue, "New Map", font))
	{
		windowToggled = !windowToggled;
	}

	GUI::Button(saveMapID, &buttonRects[1], green, "Save Map", font);

	GUI::Button(openMapID, &buttonRects[2], blue, "Open Map", font);

	GUI::Button(loadTileSetID, &buttonRects[3], green, "Load Tileset", font);

	if (windowToggled)
	{
		int windowWidth = 300;
		int windowHeight = 150;
		SDL_Rect newMapRect{ 800 / 2 - windowWidth / 2,600 / 2 - windowHeight / 2,windowWidth,windowHeight };
		GUI::Window(__LINE__, &newMapRect, font, NewMapWindow);
	}


	return false;
}

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources/");

	//load in font
	string fontPath = mainPath + string("SourceCodePro-Black.ttf");
	int fontSize = 12;
	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);

	//load in sample tilesheet to use
	ImageStore imageStore(render);
	string imageFile("tiles-stones.png");
	string imagePath(mainPath + imageFile);
	Image* src = imageStore.Load(imageFile, imagePath);
	int frameWidth = 64, frameHeight = 64;
	SpriteSheet tileSheet(render, src, frameWidth, frameHeight);

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

	int selectedButton = -1;
	SDL_Rect newWindowRect;
	newWindowRect.w = 800 * 0.3f;
	newWindowRect.h = 600 * 0.35f;
	newWindowRect.x = 400 - newWindowRect.w / 2;
	newWindowRect.y = 300 - newWindowRect.h / 2;

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

		//GUI::Window(__LINE__, &toolbarRect, font, ToolbarPrototype);

		//toolbar temp
		vector<string> strings;
		strings.push_back("New");
		strings.push_back("Open");
		strings.push_back("Save");
		int buttonToSelect = GUI::Toolbar(__LINE__,&toolbarRect, -1, strings,font);
		if (buttonToSelect != -1)
		{
			windowToggled = true;
			selectedButton = buttonToSelect;
		}

		if (!windowToggled)
		{
			selectedButton = -1;
		}
		enum TOOLMAP_BUTTONS { NEW, OPEN, SAVE };
		switch (selectedButton)
		{
		case NEW:
			GUI::Window(__LINE__, &newWindowRect, font, NewMapWindow);
			break;
		case OPEN:
			break;
		case SAVE:
			break;
		}

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