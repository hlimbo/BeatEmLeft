/*
	Testing out: Immediate Mode GUI approach
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "../MasterHeader.h"

struct ui_state
{
	//the id of the widget the mouse cursor is over on
	int hoveredID;
	//the id of the widget the mouse cursor has pressed
	int pressedID;
} ui_global_state;

//draws the button relative to the game's window
//returns true if button is pressed, otherwise it returns false
bool drawButton(SDL_Renderer* render,int ui_id, const SDL_Rect* bounds, const SDL_Color& color,SDL_Texture* text)
{
	SDL_Point mousePos;
	bool mousePressed = SDL_GetMouseState(&mousePos.x, &mousePos.y) & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (SDL_PointInRect(&mousePos, bounds))
	{
		ui_global_state.hoveredID = ui_id;
		if (mousePressed)
		{
			ui_global_state.pressedID = ui_id;
		}
	}

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

	//is pressed
	if (ui_global_state.pressedID == ui_id && ui_global_state.hoveredID == ui_id)
	{
		SDL_SetRenderDrawColor(render, color.r / 2, color.g / 2, color.b / 2, color.a);
		SDL_RenderFillRect(render, bounds);
	}
	else if (ui_global_state.hoveredID == ui_id) // hovered over
	{
		SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(render, bounds);
		SDL_SetRenderDrawColor(render, 255, 0, 255, 255 / 2);
		SDL_RenderFillRect(render, bounds);
	}
	else //inactive
	{
		SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(render, bounds);
	}

	//render text
	SDL_Rect textArea;
	SDL_QueryTexture(text, NULL, NULL, &textArea.w,&textArea.h);
	textArea.x = bounds->x + (bounds->w - textArea.w) / 2;
	textArea.y = bounds->y + (bounds->h - textArea.h) / 2;
	SDL_RenderCopy(render, text, NULL, &textArea);

	//set color back to black after drawing
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);

	//evaluate if the button was pressed by checking to see if the button is released this frame
	if (ui_global_state.pressedID == ui_id && ui_global_state.hoveredID == ui_id && !mousePressed)
		return true;

	//button was not pressed
	return false;
}

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	ImageStore store(render);

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	string fontPath = mainPath + string("Rubik_Mono_One/RubikMonoOne-Regular.ttf");
	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), 16);
	SDL_Color textColor{ 0,0,0 };
	const char* const sampleText = "OK";

	

	SDL_Surface* textSurface = TTF_RenderText_Blended(font, sampleText, textColor);
	SDL_Texture* textTextureSolid = SDL_CreateTextureFromSurface(render, textSurface);
	SDL_FreeSurface(textSurface);


	SDL_Point mousePos{ 0,0 };
	bool mouseClicked = false;

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
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEMOTION:
				mousePos.x = event.button.x;
				mousePos.y = event.button.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseClicked = true;
				break;
			case SDL_MOUSEBUTTONUP:
				mouseClicked = false;
				break;
			}
		}

		//preparation state ~ unconditionally changes the hoveredID of a widget to nothing every frame
		ui_global_state.hoveredID = 0;

		SDL_Rect buttonArea2;
		buttonArea2.w = 100;
		buttonArea2.h = 20;
		buttonArea2.x = SCREEN_WIDTH / 2 - buttonArea2.w / 2;
		buttonArea2.y = SCREEN_HEIGHT / 2 - buttonArea2.h / 2;
		int ui_id = 1;
		if (drawButton(render, ui_id, &buttonArea2, SDL_Color{ 255,255,0,255 },textTextureSolid))
		{
			//logic where button does something when clicked on
			puts("button pressed");
		}
		
		SDL_RenderPresent(render);
		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_RenderClear(render);

		//finish state ~ reset the state of being pressed to nothing if the button being pressed was released
		if (!mouseClicked)
		{
			ui_global_state.pressedID = 0;
		}

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

	SDL_DestroyTexture(textTextureSolid);
	TTF_CloseFont(font);
	font = NULL;

	return 0;
}