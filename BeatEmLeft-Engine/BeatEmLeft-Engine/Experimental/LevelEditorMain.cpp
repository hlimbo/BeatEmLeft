///*
//	LevelEditorMain.cpp
//	This program is work in progress and can only demo the following:
//		1. loads tileset from file system (loads from debug/resources folder as this entire project was built using visual studio 2017)
//		2. previews the tile selected on preview widget
//		3. displays the tiles loaded on the grid selector widget.
//
//*/
//
//#include <SDL.h>
//#include <SDL_ttf.h>
//#include <fstream>
//#include "../MasterHeader.h"
//using namespace std;
//
//#define SCREEN_WIDTH 1000
//#define SCREEN_HEIGHT 620
//
////initialization and shutdown of SDL
//static Core core;
//static SDL_Renderer* render = core.getRenderer();
//
////image store
//static ImageStore imageStore(render);
//static SpriteSheet* tileSheet = nullptr;
//
////temp ~ tile map 2d array representation
//static int** tilemap = nullptr;
//
////color pallette
//static SDL_Color yellow{ 255,231,76,255 };
//static SDL_Color red{ 255,89,100,255 };
//static SDL_Color white{ 255,255,255,255 };
//static SDL_Color blue{ 53,167,255,255 };
//static SDL_Color green{ 56,230,140,255 };
//
////main path for saving/loading resources
//static string mainPath(SDL_GetBasePath() + string("resources/"));
//
////helper function for setting the color
//static void SetDrawColor(SDL_Renderer* render, const SDL_Color& color)
//{
//	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
//}
//
////temp global variables to retain state for gui elements that will be contained within a window
////e.g. sliders and toggles need to retain state 
//static float vertSliderValue = 0.0f;
//
////todo: find a way to remove TTF_Font* parameter pass to all GUI related functions that need to display text
////e.g. GUI::Label() and GUI::TextField()
//static bool WindowFunction(int ui_id,const SDL_Rect* relativePos,TTF_Font* font)
//{
//	SDL_Rect buttonRect{ relativePos->x + 100 - 75 / 2,relativePos->y + 100 - 20 / 2,75,20 };
//	SDL_Color blue2{ 0,0,255,255 };
//	int buttonID = __LINE__;
//	if (GUI::Button(buttonID, &buttonRect, blue2, "OK", font))
//	{
//		printf("Window ID: %d\nButton ID: %d\n", ui_id, buttonID);
//		puts("Button pressed from window");
//	}
//
//	SDL_Rect vertSliderRect{relativePos->x + 10,relativePos->y + 10,20,150 };
//	int sliderID = __LINE__;
//	vertSliderValue = GUI::VerticalSlider(sliderID, &vertSliderRect, vertSliderValue, SDL_Color{ 255,0,0,255 });
//
//	//temp ~ if window doesn't have exclusive focus with the mice (i.e. the mouse is pressing on other gui elements in the window)
//	//the window should not be dragged
//	return GUI::ui_global_state.keyboardFocusID != ui_id;
//}
//
//static string mapName;
//static string levelWidth;
//static string levelHeight;
//static string tileWidth;
//static string tileHeight;
//static bool windowToggled = false;
////prototyped-canned code temporary
//static bool NewMapWindow(int ui_id, const SDL_Rect* relativePos, TTF_Font* font)
//{
//	int fontHeight = TTF_FontHeight(font);
//	int titleYOffset = 10;
//	int leftMargin = 10;
//
//	int labelID = __LINE__;
//	int titleWidth, titleHeight;
//	TTF_SizeText(font, "New Map", &titleWidth, &titleHeight);
//	//center title label along x-axis
//	SDL_Point titlePos{ relativePos->x + (relativePos->w / 2 - titleWidth / 2), relativePos->y + titleYOffset};
//	GUI::Label(labelID, &titlePos, font, "New Map", white);
//
//	//map filename field
//	int yOffset = (titlePos.y - relativePos->y) + titleHeight;
//	SDL_Point labelPos{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
//	GUI::Label(__LINE__, &labelPos, font, "Map Filename: ", white);
//
//	int textFieldID = __LINE__;
//	int widthOffset;
//	TTF_SizeText(font, "Map Filename: ", &widthOffset, NULL);
//	SDL_Rect textFieldPos{ relativePos->x + widthOffset + leftMargin,labelPos.y - 2,relativePos->w * 0.5f,fontHeight + 4};
//	mapName = GUI::TextField(textFieldID, &textFieldPos, mapName, white, font);
//
//	//level dimensions field
//	yOffset = textFieldPos.y - relativePos->y + titleHeight;
//	int levelDimensionsFieldID = __LINE__;
//	SDL_Point label2Pos{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
//	GUI::Label(levelDimensionsFieldID, &label2Pos, font, "Level WxH: ", white);
//	TTF_SizeText(font, "Level WxH: ", &widthOffset, NULL);
//	SDL_Rect textFieldPos2{ relativePos->x + leftMargin + widthOffset,label2Pos.y - 2,relativePos->w * 0.2f,fontHeight + 4 };
//	levelWidth = GUI::TextField(__LINE__, &textFieldPos2, levelWidth, white, font);
//	SDL_Point label3Pos{ relativePos->x + leftMargin + textFieldPos2.w + widthOffset,textFieldPos2.y };
//	GUI::Label(__LINE__, &label3Pos, font, " x ", white);
//	TTF_SizeText(font, " x ", &widthOffset, NULL);
//	SDL_Rect textFieldPos3{ label3Pos.x + widthOffset, textFieldPos2.y,relativePos->w * 0.2f,fontHeight + 4 };
//	levelHeight = GUI::TextField(__LINE__, &textFieldPos3, levelHeight, white, font);
//	
//	//tile dimensions field
//	yOffset = textFieldPos3.y - relativePos->y + titleHeight;
//	SDL_Point label4Pos{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
//	GUI::Label(__LINE__, &label4Pos, font, "Tile WxH: ", white);
//	TTF_SizeText(font, "Tile WxH: ", &widthOffset, NULL);
//	SDL_Rect textFieldPos4{ relativePos->x + leftMargin + widthOffset,label4Pos.y - 2,relativePos->w * 0.2f,fontHeight + 4 };
//	tileWidth = GUI::TextField(__LINE__, &textFieldPos4, tileWidth, white, font);
//	SDL_Point label5Pos{ relativePos->x + leftMargin + textFieldPos4.w + widthOffset,textFieldPos4.y };
//	GUI::Label(__LINE__, &label5Pos, font, " x ", white);
//	TTF_SizeText(font, " x ", &widthOffset, NULL);
//	SDL_Rect textFieldPos6{ label5Pos.x + widthOffset, textFieldPos4.y,relativePos->w * 0.2f,fontHeight + 4 };
//	tileHeight = GUI::TextField(__LINE__, &textFieldPos6, tileHeight, white, font);
//
//
//	int buttonWidth = relativePos->w * 0.2;
//	int buttonHeight = relativePos->h * 0.2;
//	SDL_Rect buttonRect{ relativePos->x + relativePos->w / 2 - buttonWidth / 2,relativePos->y + relativePos->h * 0.7,buttonWidth,buttonHeight};
//	if (GUI::Button(__LINE__, &buttonRect, red, "OK", font))
//	{
//		//temporary functionality as this can be dangerous to do since creating a file that already exists
//		//in the file system overrides that file!
//		puts("Close this window and save the new file created");
//		windowToggled = false;
//
//
//		string filePath(mainPath + mapName);
//		ofstream outputFile;
//		outputFile.open(filePath, ifstream::out);
//
//		if (outputFile.good())
//		{
//			//write map level name
//			outputFile << mapName << endl;
//			//write level dimensions
//			outputFile << levelWidth << " " << levelHeight << endl;
//			//write tile dimensions
//			outputFile << tileWidth << " " << tileHeight << endl;
//			outputFile.close();
//		}
//		else
//		{
//			printf("File is bad\n");
//		}
//	}
//	
//
//	return false;
//}
//
//static string tileSetName;
//static string sliceWidth;
//static string sliceHeight;
//static bool TileSetWindow(int ui_id, const SDL_Rect* relativePos, TTF_Font* font)
//{
//	int fontHeight = TTF_FontHeight(font);
//	int titleYOffset = 10;
//	int leftMargin = 10;
//
//	int titleWidth, titleHeight;
//	TTF_SizeText(font, "Load Tile Set", &titleWidth, &titleHeight);
//	//center title label along x-axis
//	SDL_Point titlePos{ relativePos->x + (relativePos->w / 2 - titleWidth / 2), relativePos->y + titleYOffset };
//	GUI::Label(__LINE__, &titlePos, font, "Load Tile Set", white);
//
//	int yOffset = (titlePos.y - relativePos->y) + titleHeight;
//	SDL_Point labelPos{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
//	GUI::Label(__LINE__, &labelPos, font, "Tile Set File: ", white);
//	int widthOffset;
//	TTF_SizeText(font, "Tile Set File:  ", &widthOffset, NULL);
//	SDL_Rect textFieldPos{ relativePos->x + widthOffset,labelPos.y - 2,relativePos->w * 0.5f,fontHeight + 4 };
//	tileSetName = GUI::TextField(__LINE__, &textFieldPos, tileSetName, white, font);
//
//	yOffset = (labelPos.y - relativePos->y) + titleHeight;
//	SDL_Point labelPos2{relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset};
//	GUI::Label(__LINE__, &labelPos2, font, "Slice Width: ", white);
//	TTF_SizeText(font, "Slice Width:  ", &widthOffset, NULL);
//	SDL_Rect textFieldPos2{ relativePos->x + widthOffset,labelPos2.y - 2,relativePos->w * 0.25f,fontHeight + 4 };
//	sliceWidth = GUI::TextField(__LINE__, &textFieldPos2, sliceWidth, white, font);
//
//	yOffset = (labelPos2.y - relativePos->y) + titleHeight;
//	SDL_Point labelPos3{ relativePos->x + leftMargin,relativePos->y + fontHeight + yOffset };
//	GUI::Label(__LINE__, &labelPos3, font, "Slice Height: ", white);
//	TTF_SizeText(font, "Slice Height:  ", &widthOffset, NULL);
//	SDL_Rect textFieldPos3{ relativePos->x + widthOffset,labelPos3.y - 2,relativePos->w * 0.25f,fontHeight + 4 };
//	sliceHeight = GUI::TextField(__LINE__, &textFieldPos3, sliceHeight, white, font);
//
//	int buttonWidth = relativePos->w * 0.2;
//	int buttonHeight = relativePos->h * 0.2;
//	SDL_Rect buttonRect{ relativePos->x + relativePos->w / 2 - buttonWidth / 2,relativePos->y + relativePos->h * 0.7,buttonWidth,buttonHeight };
//	if (GUI::Button(__LINE__, &buttonRect, red, "OK", font))
//	{
//		windowToggled = false;
//
//		if (tileSheet != nullptr)
//		{
//			delete tileSheet;
//			tileSheet = nullptr;
//		}
//
//		printf("Loading tileset: %s\n", tileSetName.c_str());
//		string filePath(mainPath + tileSetName);
//		//load tileset from file system
//		Image* img_src = imageStore.Load(tileSetName, filePath);
//		//create spritesheet from img_src
//		int frameWidth = stoi(sliceWidth);
//		int frameHeight = stoi(sliceHeight);
//		tileSheet = new SpriteSheet(render, img_src, frameWidth, frameHeight);
//	}
//
//	return false;
//}
//
//static bool TileMapWindow(int ui_id, const SDL_Rect* relativePos, TTF_Font* font)
//{
//	//draw tilemap grid
//	
//	//temporarily hardcode tile width and tile height
//	int tileWidth = 64, tileHeight = 64;
//	int windowWidth = relativePos->w, windowHeight = relativePos->h;
//
//	int numLinesAcross = windowWidth / tileWidth;
//	int numLinesBelow = windowHeight / tileHeight;
//
//	for (int v = 0;v < numLinesAcross + 1;++v)
//	{
//		SDL_RenderDrawLine(render,relativePos->x + v * tileWidth,relativePos->y,relativePos->x + v * tileWidth,relativePos->y + windowHeight);
//	}
//
//	for (int h = 0;h < numLinesBelow + 1;++h)
//	{
//		SDL_RenderDrawLine(render, relativePos->x,relativePos->y + h * tileHeight, relativePos->x + windowWidth,relativePos->y + h * tileHeight);
//	}
//
//	return false;
//}
//
//int main(int argc, char* argv[])
//{
//	//load in font
//	string fontPath = mainPath + string("SourceCodePro-Black.ttf");
//	int fontSize = 12;
//	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
//
//	//positions of each panel
//	SDL_Rect toolbarRect;
//	toolbarRect.x = 0;
//	toolbarRect.y = 0;
//	toolbarRect.w = SCREEN_WIDTH;
//	toolbarRect.h = 20;
//	SDL_Rect levelWindowRect{ 0,20,800,600 };
//	SDL_Rect sidePanelRect{ 800,20,400,600 };
//
//	SDL_Rect tilePreviewRect{ 825,50,150,150 };
//	SDL_Rect tileSetRect{ 805,250, 190, 360 };
//
//	SDL_Rect guiWindowRect{ 100,100,200,200 };
//
//	bool isToggled = true;
//	SDL_Rect toggleBounds{ 100,50, 25,25 };
//
//	float sliderValue2 = 0.0f;
//	SDL_Rect sliderRect2{ 400,50,20,200 };
//
//#define EDITOR_WIDTH 800
//#define EDITOR_HEIGHT 600
//	int selectedButton = -1;
//	SDL_Rect newWindowRect;
//	newWindowRect.w = EDITOR_WIDTH * 0.3f;
//	newWindowRect.h = EDITOR_HEIGHT * 0.35f;
//	newWindowRect.x = EDITOR_WIDTH / 2 - newWindowRect.w / 2;
//	newWindowRect.y = EDITOR_HEIGHT / 2 - newWindowRect.h / 2;
//
//	SDL_Rect tileSetWindowRect;
//	tileSetWindowRect.w = EDITOR_WIDTH * 0.35f;
//	tileSetWindowRect.h = EDITOR_HEIGHT * 0.35f;
//	tileSetWindowRect.x = EDITOR_WIDTH / 2 - tileSetWindowRect.w / 2;
//	tileSetWindowRect.y = EDITOR_HEIGHT / 2 - tileSetWindowRect.h / 2;
//
//	//tile selector
//	int selectedTileIndex = -1;
//
//	float currentTime = 0.0f;
//	float pastTime = 0.0f;
//	float printDelay = 2000.0f;
//	tilemap = (int**)malloc(sizeof(int*) * (EDITOR_HEIGHT / 64));
//	for (int i = 0;i < EDITOR_HEIGHT / 64;++i)
//	{
//		tilemap[i] = (int*)malloc(sizeof(int) * (EDITOR_WIDTH / 64));
//		for (int c = 0;c < EDITOR_WIDTH / 64;++c)
//			tilemap[i][c] = 0;
//	}
//	//---------------- Game Loop ------------------//
//
//	GUI::Init(render);
//	GameLoop::InitTimer();
//	bool running = true;
//	while (running)
//	{
//		SDL_Event event;
//		while (SDL_PollEvent(&event))
//		{
//			GUI::ProcessEvent(&event);
//			switch (event.type)
//			{
//			case SDL_QUIT:
//				running = false;
//				break;
//			}
//		}
//
//		/* GAME LOGIC FUNCTIONS GO HERE*/
//
//		//Task : draw a mockup of where all the panels will go on the tile map editor window		
//		SetDrawColor(render, blue);
//		SDL_RenderFillRect(render, &toolbarRect);
//		
//		SetDrawColor(render, white);
//		SDL_RenderFillRect(render, &levelWindowRect);
//
//		SetDrawColor(render, red);
//		SDL_RenderFillRect(render, &sidePanelRect);
//		
//		SetDrawColor(render, yellow);
//		SDL_RenderFillRect(render, &tilePreviewRect);
//
//		SetDrawColor(render, green);
//		SDL_RenderFillRect(render, &tileSetRect);
//
//		//display tile map window
//		GUI::Window(__LINE__, &levelWindowRect, font, TileMapWindow,white);
//
//		//toolbar temp
//		vector<string> strings;
//		strings.push_back("New");
//		strings.push_back("Open");
//		strings.push_back("Save");
//		strings.push_back("Load TileSet");
//		int buttonToSelect = GUI::Toolbar(__LINE__,&toolbarRect, -1, strings,font);
//		if (buttonToSelect != -1)
//		{
//			windowToggled = true;
//			selectedButton = buttonToSelect;
//		}
//
//		if (!windowToggled)
//			selectedButton = -1;
//
//		enum TOOLMAP_BUTTONS { NEW, OPEN, SAVE,LOAD_TILESET };
//		switch (selectedButton)
//		{
//		case NEW:
//			GUI::Window(__LINE__, &newWindowRect, font, NewMapWindow);
//			break;
//		case OPEN:
//			break;
//		case SAVE:
//			break;
//		case LOAD_TILESET:
//			GUI::Window(__LINE__, &tileSetWindowRect, font, TileSetWindow);
//			break;
//		}
//
//		//display tilesheet
//		if (tileSheet != nullptr)
//		{
//			int tileIndex = GUI::GridSelector(__LINE__, &tileSetRect, tileSheet, 3);
//			if (tileIndex != -1)
//				selectedTileIndex = tileIndex;
//
//			//draw tile selected preview
//			if (selectedTileIndex != -1)
//				SDL_RenderCopy(render, tileSheet->texture, tileSheet->GetFrame(selectedTileIndex), &tilePreviewRect);
//		}
//		else
//			selectedTileIndex = -1;
//
//		if (selectedTileIndex != -1)
//		{
//			//print out tilemap
//			if (currentTime > pastTime + printDelay)
//			{
//				pastTime = currentTime;
//				printf("tile Map:\n");
//				for (int r = 0;r < EDITOR_HEIGHT / 64;++r)
//				{
//					printf("{ ");
//					for (int c = 0;c < EDITOR_WIDTH / 64;++c)
//						printf("%d ", tilemap[r][c]);
//					printf(" }\n");
//				}
//
//				puts("\n\n");
//			}
//
//			//place the tile on mouse click
//
//			//use screen position coordinates temporarily
//
//		}
//
//		SDL_RenderPresent(render);
//		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
//		SDL_RenderClear(render);
//
//		GameLoop::UpdateFPS();
//		GameLoop::CapFramerate(core.getTargetDeltaTime());
//		currentTime = GameLoop::UpdateCurrentTime();
//		GameLoop::DisplayFPS(core.getWindow(), 500.0f);
//
//		GUI::SetTimeAndOldMousePos(currentTime);
//	}
//	
//	TTF_CloseFont(font);
//
//	for (int i = 0;i < EDITOR_HEIGHT / 64;++i)
//		free(tilemap[i]);
//	free(tilemap);
//	tilemap = NULL;
//
//	return 0;
//}