#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../MasterHeader.h"


int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources/");

	string fontPath = mainPath + string("SourceCodePro-Black.ttf");
	StaticTextStore textStore(render,fontPath,16);

	float vertSliderValue = 0.5f;
	float horizSliderValue = 0.1f;
	bool isToggled = true;
	string theText("Test String");
	string blankText;

	ImageStore imageStore(render);
	string imageFile("tiles-stones.png");
	string imagePath(mainPath + imageFile);
	imageStore.Load(imageFile, imagePath);

	int frameWidth = 64, frameHeight = 64;
	SpriteSheet sheet(render, imageStore.Get(imageFile), frameWidth, frameHeight);
	
	//temp
	int selected_index = -1;

	//side note: could store game loop related variables into a struct
	//the variables seen below should be initialized in the struct's constructor
	//mockup of getting target delta time:
	//GameLoop::data.targetDeltaTime;

	//---------------- Game Loop ------------------//
	//observedDeltaTime is measured in milliseconds
	float observedDeltaTime = core.getTargetDeltaTime();
	float deltaTime = observedDeltaTime / 1000.0f;//converts from milliseconds to seconds
	//to avoid unnecessary context switches os might do (which I have no control over.. cache the target delta time)
	float targetDeltaTime = core.getTargetDeltaTime();
	Uint64 observedFPS = core.getTargetFPS();
	float currentTime = 0.0f;
	float updateFPSDelay = 500.0f;//milliseconds
	float pastTime = 0.0f;
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
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

		//GUI function tests
	/*	SDL_Rect vertSliderRect{ 300, 150, 20, 160 };
		SDL_Color blue{ 0,0,255,255 };
		vertSliderValue = GUI::VerticalSlider(render, __LINE__, &vertSliderRect, vertSliderValue, blue);

		SDL_Rect horizSliderRect{ 50, 50, 160, 20 };
		SDL_Color green{ 0,255,0,255 };
		horizSliderValue = GUI::HorizontalSlider(render, __LINE__, &horizSliderRect, horizSliderValue, green);

		SDL_Rect textBoxRect{ 20,400,120 +(21 * 1.5),TTF_FontHeight(textStore.font) * 1.5f };
		SDL_Color red{ 255,0,0,255 };
		theText = GUI::TextField(render, __LINE__, &textBoxRect, theText, red, textStore.font);

		SDL_Rect tBoxRect2{ 20,350,120,TTF_FontHeight(textStore.font) * 1.5f };
		SDL_Color blue2{ 0,255,0,255 };
		blankText = GUI::TextField(render, __LINE__, &tBoxRect2,blankText, blue2, textStore.font);

		SDL_Color purple{ 255,0,255,255 };
		SDL_Point labelPos{ 200,350 };
		GUI::Label(render, __LINE__, &labelPos, textStore.font, "This is some random text", purple);
		*/

		//toggle
		SDL_Rect toggleArea{ 250, 50, 20, 20 };
		SDL_Color white{ 255,255,255,255 };
		isToggled = GUI::Toggle(render, __LINE__, &toggleArea, isToggled, white);

		//grid selector
		SDL_Rect gridBounds{ 10,SCREEN_HEIGHT - 250,240,240 };

		int index = GUI::GridSelector(render, __LINE__, &gridBounds, &sheet, 5);
		if (index != -1)
		{
			printf("index selected: %d\n", index);
			selected_index = index;
		}

		//temp draw selected tile
		SDL_Rect dstRect{ 300,SCREEN_HEIGHT - 250,128,128 };
		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		SDL_RenderDrawRect(render, &dstRect);
		if (selected_index != -1)
		{
			SDL_RenderCopy(render, sheet.texture, sheet.GetFrame(selected_index),&dstRect);
		}

		//button
		SDL_Rect buttonArea{ 85, 100, 100, 20 };
		SDL_Color neonBlue{ 103, 200, 255, 255 };
		if (GUI::Button(render, __LINE__, &buttonArea, neonBlue, "GO", textStore.font))
		{
			puts("neon blue button pressed");
		}

		SDL_RenderPresent(render);
		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_RenderClear(render);

		endCount = SDL_GetPerformanceCounter();
		observedDeltaTime = (1000.0f * (endCount - startCount)) / performanceFrequency;//gives ms
		observedFPS = performanceFrequency / (endCount - startCount);

		//cap framerate
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

		//update game time
		currentTime += observedDeltaTime;
		deltaTime = observedDeltaTime / 1000.0f;
		startCount = endCount;

		//mockup of what the functions would look like.
		//GameLoop::UpdateFPS();
		//GameLoop::CapFramerate(float targetDeltaTime);
		//GameLoop::UpdateCurrentTime();

		//todo: should be wrapped in some function? e.g. GUI::PreProcessData();
		GUI::ui_global_state.currentTime = currentTime;
		GUI::ui_global_state.oldMousePos.x = GUI::ui_global_state.mousePos.x;
		GUI::ui_global_state.oldMousePos.y = GUI::ui_global_state.mousePos.y;


		//mockup of what display framerate function would look like
		//GameLoop::DisplayFPS(SDL_Window* window,const string FPS,int updateDelayInMS);
		if (currentTime - pastTime >= updateFPSDelay)
		{
			pastTime = currentTime;
			//display fps text in title
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}
	}


	return 0;
}