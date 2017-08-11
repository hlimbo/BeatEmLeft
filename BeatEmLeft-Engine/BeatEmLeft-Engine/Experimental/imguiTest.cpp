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
	ui_global_state.hoveredID = 0;

	//changes the global state depending on these conditions
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

	//this updates how the button is drawn depending on what the global state is in
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
	{
		ui_global_state.pressedID = 0;
		return true;
	}

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
	SDL_Point oldMousePos{ 0,0 };
	bool mouseClicked = false;

	//initial slider properties
	SDL_Color scrollWheelColor = { 0,0,255,255 };
	SDL_Rect scrollWheelRect;
	scrollWheelRect.x = 100;
	scrollWheelRect.y = 50;
	scrollWheelRect.w = 20;
	scrollWheelRect.h = 20;

	SDL_Color scrollBarColor = { 255,255,255,255 };
	SDL_Rect scrollBarRect;
	scrollBarRect.x = 100;
	scrollBarRect.y = 50;
	scrollBarRect.w = 20;
	scrollBarRect.h = 160;

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

		SDL_Rect buttonArea;
		buttonArea.w = 100;
		buttonArea.h = 20;
		buttonArea.x = 85;
		buttonArea.y = 230;
		if (drawButton(render, 1, &buttonArea, SDL_Color{ 0,255,0,255 }, textTextureSolid))
		{
			puts("button pressed 1");
		}

		SDL_Rect buttonArea2;
		buttonArea2.w = 100;
		buttonArea2.h = 20;
		buttonArea2.x = SCREEN_WIDTH / 2 - buttonArea2.w / 2;
		buttonArea2.y = SCREEN_HEIGHT / 2 - buttonArea2.h / 2;
		if (drawButton(render, 2, &buttonArea2, SDL_Color{ 255,255,0,255 },textTextureSolid))
		{
			//logic where button does something when clicked on
			puts("button pressed 2");
		}

		//draw a slider
		int slider_id = 3;

		//check if mouse is on scrollbar
		if (SDL_PointInRect(&mousePos, &scrollBarRect))
		{
			ui_global_state.hoveredID = slider_id;
			if (mouseClicked && ui_global_state.pressedID == 0)
			{
				ui_global_state.pressedID = slider_id;
			}
		}

		SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

		//draw scroll bar
		SDL_SetRenderDrawColor(render, scrollBarColor.r, scrollBarColor.g, scrollBarColor.b, scrollBarColor.a);
		SDL_RenderFillRect(render, &scrollBarRect);

		//1st goal: keep scroll knob in place anywhere that it is clicked on(do not move the knob if mouse delta y == 0)
		//2nd goal: move the scroll knob when mouse delta y != 0;

		//is pressed
		if (ui_global_state.pressedID == slider_id)
		{
			ui_global_state.pressedID = 0;

			int deltaMousePos = mousePos.y - oldMousePos.y;
			if (deltaMousePos != 0)
			{
				int projectedPos = scrollWheelRect.y + deltaMousePos;
				//3rd goal: keep the scroll knob in place when it touches the top bounds
				if (projectedPos < scrollBarRect.y)
					scrollWheelRect.y = scrollBarRect.y;
				//4th goal: keep the scroll knob in place when it touches the bottom bounds
				else if (projectedPos + scrollWheelRect.h > scrollBarRect.y + scrollBarRect.h)
					scrollWheelRect.y = (scrollBarRect.y + scrollBarRect.h) - scrollWheelRect.h;
				else
					scrollWheelRect.y = projectedPos;

			}

			//render scroll wheel
			SDL_SetRenderDrawColor(render, scrollWheelColor.r / 2, scrollWheelColor.g / 2, scrollWheelColor.b / 2, scrollWheelColor.a);
			SDL_RenderFillRect(render, &scrollWheelRect);
		}
		else //not pressed
		{
			//render scroll wheel
			SDL_SetRenderDrawColor(render, scrollWheelColor.r, scrollWheelColor.g, scrollWheelColor.b, scrollWheelColor.a);
			SDL_RenderFillRect(render, &scrollWheelRect);
		}

		oldMousePos.x = mousePos.x;
		oldMousePos.y = mousePos.y;
		

		SDL_RenderPresent(render);
		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_RenderClear(render);

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