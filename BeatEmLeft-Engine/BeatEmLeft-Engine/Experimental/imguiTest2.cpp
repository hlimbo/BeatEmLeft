///*
//	imguiTest2.cpp further demos guis I wrote procedurally in GUI.cpp
//
//	It now features a grid selector and grid preview  widgets
//	to showcase how they work in action.
//
//	Controls:
//		Use your mouse to hover over a tile and left click to select the tile
//		to preview in the preview widget.
//*/
//
//#include <SDL.h>
//#include <SDL_ttf.h>
//#include "../MasterHeader.h"
//
//
//int main(int argc, char* argv[])
//{
//	Core core;
//	SDL_Renderer* render = core.getRenderer();
//	
//	string mainPath(SDL_GetBasePath());
//	mainPath += string("resources/");
//
//	string fontPath = mainPath + string("SourceCodePro-Black.ttf");
//	StaticTextStore textStore(render,fontPath,16);
//
//	float vertSliderValue = 0.5f;
//	float horizSliderValue = 0.1f;
//	bool isToggled = true;
//	string theText("Test String");
//	string blankText;
//
//	ImageStore imageStore(render);
//	string imageFile("tiles-stones.png");
//	string imagePath(mainPath + imageFile);
//	imageStore.Load(imageFile, imagePath);
//
//	int frameWidth = 64, frameHeight = 64;
//	SpriteSheet sheet(render, imageStore.Get(imageFile), frameWidth, frameHeight);
//	
//	//temp
//	int selected_index = -1;
//
//	//side note: could store game loop related variables into a struct
//	//the variables seen below should be initialized in the struct's constructor
//	//mockup of getting target delta time:
//	//GameLoop::data.targetDeltaTime;
//
//	//---------------- Game Loop ------------------//
//	//observedDeltaTime is measured in milliseconds
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
//			
//		}
//
//		//GUI function tests
//	/*	SDL_Rect vertSliderRect{ 300, 150, 20, 160 };
//		SDL_Color blue{ 0,0,255,255 };
//		vertSliderValue = GUI::VerticalSlider(__LINE__, &vertSliderRect, vertSliderValue, blue);
//
//		SDL_Rect horizSliderRect{ 50, 50, 160, 20 };
//		SDL_Color green{ 0,255,0,255 };
//		horizSliderValue = GUI::HorizontalSlider(__LINE__, &horizSliderRect, horizSliderValue, green);
//
//		SDL_Rect textBoxRect{ 20,400,120 +(21 * 1.5),TTF_FontHeight(textStore.font) * 1.5f };
//		SDL_Color red{ 255,0,0,255 };
//		theText = GUI::TextField(__LINE__, &textBoxRect, theText, red, textStore.font);
//
//		SDL_Color purple{ 255,0,255,255 };
//		SDL_Point labelPos{ 200,350 };
//		GUI::Label(__LINE__, &labelPos, textStore.font, "This is some random text", purple);
//		*/
//
//		//toggle
//		SDL_Rect toggleArea{ 250, 50, 20, 20 };
//		SDL_Color white{ 255,255,255,255 };
//		isToggled = GUI::Toggle(__LINE__, &toggleArea, isToggled, white);
//
//		//grid selector
//		SDL_Rect gridBounds{ 10,SCREEN_HEIGHT - 250,240,240 };
//
//		int index = GUI::GridSelector(__LINE__, &gridBounds, &sheet, 5);
//		if (index != -1)
//		{
//			printf("index selected: %d\n", index);
//			selected_index = index;
//		}
//
//		//temp draw selected tile
//		SDL_Rect dstRect{ 300,SCREEN_HEIGHT - 250,128,128 };
//		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
//		SDL_RenderDrawRect(render, &dstRect);
//		if (selected_index != -1)
//		{
//			SDL_RenderCopy(render, sheet.texture, sheet.GetFrame(selected_index),&dstRect);
//		}
//
//		//button
//		SDL_Rect buttonArea{ 85, 100, 100, 20 };
//		SDL_Color neonBlue{ 103, 200, 255, 255 };
//		if (GUI::Button(__LINE__, &buttonArea, neonBlue, "GO", textStore.font))
//		{
//			puts("neon blue button pressed");
//		}
//
//		//text field
//		SDL_Rect tBoxRect2{ 20,300,120,TTF_FontHeight(textStore.font) * 1.5f };
//		SDL_Color blue2{ 0,255,0,255 };
//		blankText = GUI::TextField(__LINE__, &tBoxRect2, blankText, blue2, textStore.font);
//
//		SDL_RenderPresent(render);
//		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
//		SDL_RenderClear(render);
//
//		GameLoop::UpdateFPS();
//		GameLoop::CapFramerate(core.getTargetDeltaTime());
//		float currentTime = GameLoop::UpdateCurrentTime();
//
//		GUI::SetTimeAndOldMousePos(currentTime);
//		GameLoop::DisplayFPS(core.getWindow(), 500.0f);
//	}
//
//
//	return 0;
//}