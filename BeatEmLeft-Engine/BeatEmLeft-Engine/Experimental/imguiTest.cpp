/*
	Testing out: Immediate Mode GUI approach
	Note: ids for each ui object is used to distinguish which ui is being hovered on or pressed on,
	If only a single global state such as an enum was used to define the states of each button, all buttons would be affected by the change
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

	SDL_Point mousePos;
	SDL_Point oldMousePos;

	bool textChanged;
	bool mouseClicked;
	//holds the text input when SDL_TEXTINPUT event is triggered
	char* textBuffer;

	int keyboardFocusID;
	int keyPressed;
	int prevPressed;
	Uint16 keyMod;

	//measured in milliseconds since start of game loop
	float currentTime;
	float pastTime;

	//used to create blinking text cursor effect
	bool isTextCursorVisible;

	ui_state()
	{
		hoveredID = 0;
		pressedID = 0;
		mousePos = SDL_Point{ 0,0 };
		oldMousePos = SDL_Point{ 0,0 };
		mouseClicked = false;
		textChanged = false;
		keyboardFocusID = 0;
		keyPressed = -1;
		prevPressed = -1;
		currentTime = 0.0f;
		pastTime = 0.0f;
		isTextCursorVisible = true;
		textBuffer = NULL;
	}

	~ui_state() {}

} ui_global_state;


struct Text
{
	SDL_Texture* texture;
	string text;
	TTF_Font* font;
	unsigned int char_limit;

	Text()
	{
		texture = NULL;
		font = NULL;
		char_limit = -1;
		text = "";
	}

	~Text() {} 
};

//draws the button relative to the game's window
//returns true if button is pressed, otherwise it returns false
bool drawButton(SDL_Renderer* render,int ui_id, const SDL_Rect* bounds, const SDL_Color& color,SDL_Texture* text)
{
	//changes the global state depending on these conditions
	SDL_Point mousePos;
	bool mousePressed = SDL_GetMouseState(&mousePos.x, &mousePos.y) & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (SDL_PointInRect(&mousePos, bounds))
	{
		ui_global_state.hoveredID = ui_id;
		ui_global_state.keyboardFocusID = ui_id;
		if (mousePressed)
		{
			ui_global_state.pressedID = ui_id;
		}
	}

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

	//this updates how the button is colored depending on what the global state is in
	//if enter key is pressed when button has keyboard focus
	if (ui_global_state.keyboardFocusID == ui_id && ui_global_state.keyPressed == SDLK_RETURN)
	{
		ui_global_state.prevPressed = SDLK_RETURN;
		SDL_SetRenderDrawColor(render, color.r / 2, color.g / 2, color.b / 2, color.a);
		SDL_RenderFillRect(render, bounds);
	}
	//is pressed by mouse
	else if (ui_global_state.pressedID == ui_id && ui_global_state.hoveredID == ui_id)
	{
		SDL_SetRenderDrawColor(render, color.r / 2, color.g / 2, color.b / 2, color.a);
		SDL_RenderFillRect(render, bounds);
	}
	else if (ui_global_state.hoveredID == ui_id) // hovered over
	{
		SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(render, bounds);
		SDL_SetRenderDrawColor(render, color.r, color.g / 2, color.b, color.a / 2);
		SDL_RenderFillRect(render, bounds);
	}
	else //inactive
	{
		SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(render, bounds);
	}

	//render text ~ temporary placeholder text
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

	if(ui_global_state.keyboardFocusID == ui_id && ui_global_state.keyPressed == -1 && ui_global_state.prevPressed == SDLK_RETURN)
	{
		ui_global_state.prevPressed = -1;
		return true;
	}

	//button was not pressed
	return false;
}

//1st version
void drawVerticalSlider(SDL_Renderer* render, int ui_id, const SDL_Color& knobColor, const SDL_Color& barColor, SDL_Rect* knobBounds, const SDL_Rect* barBounds, const SDL_Point* mousePos)
{
	SDL_Point newMousePos;
	bool mouseClicked = SDL_GetMouseState(&newMousePos.x, &newMousePos.y) & SDL_BUTTON(SDL_BUTTON_LEFT);
	
	//determine if scroll bar is hovered on or pressed on
	if(SDL_PointInRect(mousePos,barBounds))
	{
		ui_global_state.hoveredID = ui_id;
		if (mouseClicked)
		{
			ui_global_state.pressedID = ui_id;
		}
	}

	//1st goal: keep scroll knob in place anywhere that it is clicked on(do not move the knob if mouse delta y == 0)
	//2nd goal: move the scroll knob when mouse delta y != 0;
	if (ui_global_state.pressedID == ui_id)
	{
		int deltaMousePos = newMousePos.y - mousePos->y;
		if (deltaMousePos != 0)
		{
			int projectedPos = knobBounds->y + deltaMousePos;
			//3rd goal: keep the scroll knob in place when it touches the top bounds
			if (projectedPos < barBounds->y)
				knobBounds->y = barBounds->y;
			//4th goal: keep the scroll knob in place when it touches the bottom bounds
			else if (projectedPos + knobBounds->h > barBounds->y + barBounds->h)
				knobBounds->y = (barBounds->y + barBounds->h) - knobBounds->h;
			else
				knobBounds->y = projectedPos;
		}
	}
	
	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
	//draw scroll bar
	SDL_SetRenderDrawColor(render, barColor.r, barColor.g, barColor.b, barColor.a);
	SDL_RenderFillRect(render, barBounds);

	//render knob
	if (ui_global_state.pressedID == ui_id)
	{
		ui_global_state.pressedID = 0;
		SDL_SetRenderDrawColor(render, knobColor.r / 2, knobColor.g / 2, knobColor.b / 2, knobColor.a);
		SDL_RenderFillRect(render, knobBounds);
	}
	else
	{
		SDL_SetRenderDrawColor(render, knobColor.r, knobColor.g, knobColor.b, knobColor.a);
		SDL_RenderFillRect(render, knobBounds);
	}

}

//returns the position of the knob as a floating point value mapped between a min and max float value.
//Note: initialValue must be a floating point value between 0.0f and 1.0f inclusive
//2nd version
float drawVerticalSlider(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, float initialValue)
{
	//construct the knob bounds
	float knobScale = 0.25f;
	SDL_Rect knobBounds;
	knobBounds.x = bounds->x;
	knobBounds.w = bounds->w;
	knobBounds.h = (int)(knobScale * bounds->h);

	SDL_Color knobColor{ 160, 160,160,255 };
	SDL_Color barColor{ 114, 99, 99, 255 };

	SDL_Point newMousePos;
	bool mouseClicked = SDL_GetMouseState(&newMousePos.x, &newMousePos.y) & SDL_BUTTON(SDL_BUTTON_LEFT);

	//determine if scroll bar is hovered over or pressed on
	if (SDL_PointInRect(&newMousePos, bounds))
	{
		ui_global_state.hoveredID = ui_id;
		if (mouseClicked)
		{
			ui_global_state.pressedID = ui_id;
		}
	}

	//keep the scroll knob in place anywhere that it is clicked on
	//move the scroll knob by the amount the mouse position changes (delta mouse pos)
	
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float value = (initialValue < minValue) ? minValue : (initialValue > maxValue) ? maxValue : initialValue;
	knobBounds.y = (int)(value * bounds->h) + bounds->y;
	if (ui_global_state.pressedID == ui_id)
	{
		int deltaMousePos = newMousePos.y - ui_global_state.oldMousePos.y;
		if (deltaMousePos != 0)
		{
			//deltaV ranges from -1 to 1 inclusive
			float deltaV = (float)deltaMousePos / bounds->h;
			value += deltaV;
			value = (value > maxValue) ? maxValue : (value < minValue) ? minValue : value;
		}

		//printf("value: %f\n", value);	
		knobBounds.y = (int)(value * bounds->h) + bounds->y;
	}

	//since the knob has a height and its coordinates are measured from its top-left corner, 
	//its necessary to make sure the knob doesn't move past its scrollable area.
	if (knobBounds.y + knobBounds.h > bounds->y + bounds->h)
		knobBounds.y = (bounds->y + bounds->h) - knobBounds.h;

	//rendering
	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
	//draw scroll bar
	SDL_SetRenderDrawColor(render, barColor.r, barColor.g, barColor.b, barColor.a);
	SDL_RenderFillRect(render, bounds);

	//draw scroll knob
	if (ui_global_state.pressedID == ui_id)
	{
		ui_global_state.pressedID = 0;
		SDL_SetRenderDrawColor(render, knobColor.r / 2, knobColor.g / 2, knobColor.b / 2, knobColor.a);
		SDL_RenderFillRect(render, &knobBounds);
	}
	else
	{
		SDL_SetRenderDrawColor(render, knobColor.r, knobColor.g, knobColor.b, knobColor.a);
		SDL_RenderFillRect(render, &knobBounds);
	}

	return value;
}

//label - non interactive
//not a very good function to call if drawing many labels because it takes up cpu time and lowers fps
//because surfaces and the textures of the text are being constantly deleted and created
//rendered text labels should be stored in a hashmap to prevent having to convert strings to textures every frame
void drawLabel(SDL_Renderer* render,SDL_Point screen_position,TTF_Font* font,const string& text)
{
	SDL_Color blue{ 0,0,255,255 };
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), blue);
	SDL_Texture* textBlended = SDL_CreateTextureFromSurface(render, textSurface);
	SDL_FreeSurface(textSurface);

	SDL_Rect textArea;
	textArea.x = screen_position.x;
	textArea.y = screen_position.y;
	SDL_QueryTexture(textBlended, NULL, NULL, &textArea.w, &textArea.h);
	SDL_RenderCopy(render, textBlended, NULL, &textArea);
	SDL_DestroyTexture(textBlended);

}

//to make this functional this should either return a new string with the updated text
//or an updated texture with the modified text
string drawTextField(SDL_Renderer* render, int ui_id, SDL_Point screenPos, Text* text, SDL_Color color = SDL_Color{ 255,255,255,255 })
{

	if (!TTF_FontFaceIsFixedWidth(text->font))
		printf("Warning: characters of this font style vary in font width\n");

	//construct textbox area based on character limit and font size
	int fontWidth, fontHeight;
	TTF_SizeText(text->font, "A", &fontWidth, &fontHeight);
	SDL_Rect textBoxRect;
	textBoxRect.x = screenPos.x;
	textBoxRect.y = screenPos.y;
	textBoxRect.w = (fontWidth * text->char_limit) + (2 * fontWidth);
	textBoxRect.h = fontHeight + (fontHeight / 2);

	//construct text area
	SDL_Rect textAreaRect;
	textAreaRect.x = screenPos.x + fontWidth;
	textAreaRect.y = screenPos.y + (fontHeight / 4);
	textAreaRect.w = fontWidth * text->char_limit;
	textAreaRect.h = fontHeight;
	
	//check if text field is hovered over or clicked on
	SDL_Point mousePos;
	bool mousePressed = SDL_GetMouseState(&mousePos.x, &mousePos.y) & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (SDL_PointInRect(&mousePos, &textBoxRect))
	{
		ui_global_state.hoveredID = ui_id;
		if (mousePressed)
		{
			ui_global_state.pressedID = ui_id;
			ui_global_state.keyboardFocusID = ui_id;
		}
	}
	else
	{
		if (!mousePressed)
			ui_global_state.hoveredID = 0;
		//if mouse was pressed but wasn't inside the text field, lose keyboard focus
		else if(ui_global_state.keyboardFocusID == ui_id)
			ui_global_state.keyboardFocusID = 0;
	}

	//if text field has current focus, turn on text cursor
	if (ui_global_state.keyboardFocusID == ui_id)
	{
		//puts("turn on blinker");
		ui_global_state.pressedID = 0;

		SDL_StartTextInput();

		//construct text cursor
		SDL_Rect textCursorRect;
		textCursorRect.x = textAreaRect.x;
		textCursorRect.y = textAreaRect.y - (fontHeight / 8);
		TTF_SizeText(text->font, "|", &textCursorRect.w, &textCursorRect.h);

		//check if paste shortcut key pressed (ctrl-v)
		//adds on to end of text
		if (ui_global_state.keyPressed == SDLK_v && (ui_global_state.keyMod & KMOD_CTRL))
		{
			ui_global_state.keyPressed = -1;
			string temp(SDL_GetClipboardText());
			if (temp.size() + text->text.size() < text->char_limit)
			{
				text->text += temp;
				ui_global_state.textChanged = true;
			}
		}

		//update the text texture
		if (ui_global_state.textChanged)
		{
			ui_global_state.textChanged = false;
			//check if text was input or backspace was entered
			if (ui_global_state.keyPressed == SDLK_BACKSPACE)
			{
				if (!text->text.empty())	
					text->text.pop_back();
			}
			else // text input
			{
				if (text->char_limit > text->text.size() && ui_global_state.textBuffer != NULL)
				{
					text->text += string(ui_global_state.textBuffer);
					ui_global_state.textBuffer = NULL;
				}
			}

			if (text->texture != NULL)
				SDL_DestroyTexture(text->texture);

			SDL_Surface* textSurface = TTF_RenderText_Blended(text->font, text->text.c_str(), color);
			text->texture = SDL_CreateTextureFromSurface(render, textSurface);
			SDL_FreeSurface(textSurface);

		}
		else
		{
			//check if copy shortcut key pressed (ctrl-c)
			if (ui_global_state.keyPressed == SDLK_c && (ui_global_state.keyMod & KMOD_CTRL))
			{
				ui_global_state.keyPressed = -1;
				SDL_SetClipboardText(text->text.c_str());
			}
		}

		//update position of text cursor
		TTF_SizeText(text->font, text->text.c_str(), &textAreaRect.w, NULL);
		textCursorRect.x = textAreaRect.x + textAreaRect.w;

		//blink text cursor effect
		float currentTime = ui_global_state.currentTime;
		float pastTime = ui_global_state.pastTime;
		float blinkDelay = 500.0f;
		if (currentTime - pastTime >= blinkDelay)
		{
			ui_global_state.pastTime = currentTime;
			ui_global_state.isTextCursorVisible = !ui_global_state.isTextCursorVisible;
		}
		
		//create and destroy the text cursor texture every other 500 ms
		if (ui_global_state.isTextCursorVisible)
		{
			SDL_Surface* textCursor_s = TTF_RenderText_Blended(text->font, "|", color);
			SDL_Texture* textCursor_t = SDL_CreateTextureFromSurface(render, textCursor_s);
			SDL_RenderCopy(render, textCursor_t, NULL, &textCursorRect);
			SDL_FreeSurface(textCursor_s);
			SDL_DestroyTexture(textCursor_t);
		}
	}
	else if(ui_global_state.keyboardFocusID == 0)
	{
		//puts("turn off blinker");
		SDL_StopTextInput();
	}

	//display textbox and text
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(render, &textBoxRect);

	TTF_SizeText(text->font, text->text.c_str(), &textAreaRect.w, NULL);
	if (text->texture != NULL)
		SDL_RenderCopy(render, text->texture, NULL, &textAreaRect);

	return text->text;
}

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();
	ImageStore store(render);

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");

	string fontPath = mainPath + string("SourceCodePro-Black.ttf");
	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), 16);
	SDL_Color textColor{ 0,0,0 };
	const char* const sampleText = "OK";

	SDL_Surface* textSurface = TTF_RenderText_Blended(font, sampleText, textColor);
	SDL_Texture* textTextureSolid = SDL_CreateTextureFromSurface(render, textSurface);
	SDL_FreeSurface(textSurface);

	if (TTF_FontFaceIsFixedWidth(font))
		printf("this font is fixed\n");
	else
		printf("this font is not fixed\n");

	//initial slider properties
	float initialScrollValue = 0.0f;
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

	Text textStruct;
	textStruct.char_limit = 20;
	textStruct.font = font;

	Text textStruct2;
	textStruct2.char_limit = 40;
	textStruct2.font = font;

	Text textStruct3;
	textStruct3.char_limit = 15;
	textStruct3.font = font;


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
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_MOUSEMOTION:
				ui_global_state.mousePos.x = event.button.x;
				ui_global_state.mousePos.y = event.button.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				ui_global_state.mouseClicked = true;
				printf("mousePos: (%d, %d)\n", ui_global_state.mousePos.x, ui_global_state.mousePos.y);
				break;
			case SDL_MOUSEBUTTONUP:
				ui_global_state.mouseClicked = false;
				break;
			case SDL_KEYDOWN:
				//report any key presses to the widgets.
				ui_global_state.keyPressed = event.key.keysym.sym;
				ui_global_state.keyMod = event.key.keysym.mod;
				if (event.key.keysym.sym == SDLK_BACKSPACE)
					ui_global_state.textChanged = true;
				break;
			case SDL_KEYUP:
				ui_global_state.keyPressed = -1;
				ui_global_state.keyMod = 0;
				break;
			case SDL_TEXTINPUT:
				ui_global_state.textBuffer = event.text.text;
				ui_global_state.textChanged = true;
				break;
			}
		}

		//GUI Code Testing//	

		SDL_Rect buttonArea;
		buttonArea.w = 100;
		buttonArea.h = 20;
		buttonArea.x = 85;
		buttonArea.y = 230;
		SDL_Color neonBlue{ 103,200,255,255 };
		if (drawButton(render, 2, &buttonArea, neonBlue, textTextureSolid))
		{
			puts("neon blue button pressed");
		}

		//todo: pass in a string into drawTextField to modify and display on screen
		SDL_Color blue{ 0,0,255,255 };
		textStruct.text = drawTextField(render, 7, SDL_Point{ 50,350 }, &textStruct,blue);
		textStruct2.text = drawTextField(render, 8, SDL_Point{ 50, 400 }, &textStruct2);
		SDL_Color red{ 255,0,0,255 };
		drawTextField(render, 9, SDL_Point{ 50,300 }, &textStruct3, red);

		//2nd version
		SDL_Rect bounds2;
		bounds2.x = 300;
		bounds2.y = 150;
		bounds2.w = 20;
		bounds2.h = 150;
		initialScrollValue = drawVerticalSlider(render, 4, &bounds2, initialScrollValue);

		SDL_Rect buttonArea2;
		buttonArea2.w = 100;
		buttonArea2.h = 20;
		buttonArea2.x = SCREEN_WIDTH / 2 - buttonArea2.w / 2;
		buttonArea2.y = SCREEN_HEIGHT / 2 - buttonArea2.h / 2;
		if (drawButton(render, 3, &buttonArea2, SDL_Color{ 255,255,0,255 }, textTextureSolid))
		{
			//logic where button does something when clicked on
			puts("button pressed 2");
		}

		ui_global_state.oldMousePos.x = ui_global_state.mousePos.x;
		ui_global_state.oldMousePos.y = ui_global_state.mousePos.y;

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

		ui_global_state.currentTime = currentTime;

		if(currentTime - pastTime >= updateFPSDelay)
		{
			pastTime = currentTime;
			//display fps text in title
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}

	}

	SDL_DestroyTexture(textTextureSolid);
	TTF_CloseFont(font);
	font = NULL;

	return 0;
}