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

	//following the tutorial for imgui: http://sol.gfxile.net/imgui/ch06.html
	//stores the widget's id that has keyboard focus
	int kbditem;
	//stores the key that was pressed
	int keyentered;
	//stores the key modifier flags such as shift pressed
	int keymod;

	//holds the id of the last widget procesessed.
	int lastwidget;

	//add a bool textChanged

} ui_global_state;

struct Text
{
	SDL_Rect bounds;
	string text;
	int max_len;
	SDL_Texture* texture_;
	SDL_Surface* surface_;
	SDL_Color color;
};

struct Text2
{
	SDL_Texture* texture;
	string text;
	TTF_Font* font;
	int char_limit;

	//optional ~ might not need
	SDL_Color color;
	SDL_Rect bounds;

	Text2()
	{
		texture = NULL;
		font = NULL;
		char_limit = -1;
		color = SDL_Color{ 255,255,255,255 };
	}

	~Text2() {} 

	//~Text2()
	//{
	//	if (texture != NULL)
	//	{
	//		SDL_DestroyTexture(texture);
	//		texture = NULL;
	//	}
	//	//temporary...
	///*	if (font != NULL)
	//	{
	//		TTF_CloseFont(font);
	//		font = NULL;
	//	}*/
	//	char_limit = -1;
	//}
};

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

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

	//if no widget has keyboard focus, take it
	if (ui_global_state.kbditem == 0)
		ui_global_state.kbditem = ui_id;

	//if we have keyboard focus, show it
	if (ui_global_state.kbditem == ui_id)
	{
		SDL_SetRenderDrawColor(render, 255, 0, 0, 255);
		SDL_Rect outline = *bounds;
		outline.w = 120;
		outline.h = 35;
		SDL_RenderFillRect(render, &outline);
	}

	//this updates how the button is drawn depending on what the global state is in

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


	//if we have keyboard focus, we'll need to process the keys
	if (ui_global_state.kbditem == ui_id)
	{
		switch (ui_global_state.keyentered)
		{
		case SDLK_TAB:
			//if tab is pressed, lose keyboard focus
			//next widget will grab focus
			ui_global_state.kbditem = 0;
			//if shift was also pressed, we want to move focus to the previous widget
			//instead
			if (ui_global_state.keymod & KMOD_SHIFT)
				ui_global_state.kbditem = ui_global_state.lastwidget;
			//also clear the key so that next widget won't process it
			ui_global_state.keyentered = 0;
			break;
		case SDLK_RETURN:
			//had keyboard focus, received return,so we act as if clicked happened
			return 1;
		}
	}

	ui_global_state.lastwidget = ui_id;

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
bool drawTextField(SDL_Renderer* render,int ui_id, SDL_Point screenPos, Text2* text)
{
	if (!TTF_FontFaceIsFixedWidth(text->font))
		printf("Warning: characters of this font style vary in font width\n");

	//construct textbox area based on character limit and font size
	int fontWidth, fontHeight;
	TTF_SizeText(text->font, "A", &fontWidth, &fontHeight);
	SDL_Rect textBoxRect;
	textBoxRect.x = screenPos.x;
	textBoxRect.y = screenPos.y;
	textBoxRect.w = (fontWidth * text->char_limit) + fontWidth;
	textBoxRect.h = fontHeight + (fontHeight / 2);

	//construct text area
	SDL_Rect textAreaRect;
	textAreaRect.x = screenPos.x + (int)(textBoxRect.w * 0.1f);
	textAreaRect.y = screenPos.y + (int)(textBoxRect.h * 0.05f);
	textAreaRect.w = (fontWidth * text->char_limit);
	textAreaRect.h = fontHeight;

	//check if text field is hovered over or clicked on
	ui_global_state.hoveredID = 0;

	SDL_Point mousePos;
	bool mousePressed = SDL_GetMouseState(&mousePos.x, &mousePos.y) & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (SDL_PointInRect(&mousePos, &textBoxRect))
	{
		ui_global_state.hoveredID = ui_id;
		if (mousePressed)
		{
			ui_global_state.pressedID = ui_id;
		}
	}

	//if pressed on turn on blinker
	if (ui_global_state.pressedID == ui_id)
	{
		puts("turn on blinker");
		//turn on cursor blinker
		SDL_StartTextInput();
	}
	else
	{
		puts("turn off blinker");
		SDL_StopTextInput();
	}

	//display textbox and text
	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
	SDL_RenderDrawRect(render, &textBoxRect);

	//if textChanges
	//remake text texture with text updated text

	return false;
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

	//TTF_GlyphMetrics(font,'a',)
	//TTF_SetFontStyle(font,TTF_STYLE_ITALIC);
	//TTF_SetFontOutline(font, 1);
	if (TTF_FontFaceIsFixedWidth(font))
		printf("this font is fixed\n");
	else
		printf("this font is not fixed\n");

	//char* familyName = TTF_FontFaceFamilyName(font);
	//char* styleName = TTF_FontFaceStyleName(font);
	//printf("familyName: %s\nstyleName: %s\n", familyName, styleName);

	SDL_Point mousePos{ 0,0 };
	SDL_Point oldMousePos{ 0,0 };
	bool mouseClicked = false;

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

	ui_global_state.hoveredID = 0;
	ui_global_state.pressedID = 0;
	ui_global_state.kbditem = 0;
	ui_global_state.keyentered = 0;
	ui_global_state.keymod = 0;
	ui_global_state.mousePos = mousePos;
	ui_global_state.oldMousePos = oldMousePos;

	//text input//
	Text t;
	t.text = string("");
	t.color = SDL_Color{ 255,255,255,255 };
	t.texture_ = NULL;
	t.surface_ = NULL;
	bool textChanged = false;

	//blinking cursor
	textSurface = TTF_RenderText_Blended(font, "|", SDL_Color{ 255,255,255,255 });
	SDL_Texture* blinkingTexture = SDL_CreateTextureFromSurface(render, textSurface);
	SDL_SetTextureBlendMode(blinkingTexture, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(textSurface);
	float blinkDelay = 500.0f;//milliseconds
	float pastTime = 0.0f;
	int minAlpha = 0;
	int maxAlpha = 255;
	int blinkAlpha = maxAlpha;

	/*
		Parameters needed to bool drawTextField();
		1. SDL_Renderer
		2. SDL_Point screenPos
		3. TTF_Font* font
		4. int character_limit
		5. SDL_Texture* (holds how the text typed on keyboard is rendered to screen) (gets destroyed and created frequently when text is being modified)
		6. string text

		struct Text
		{
			SDL_Texture* texture;
			char* buffer;
			int character_limit;
			TTF_Font* font;
			int curr_index; //which will be updated everytime a character is added or removed from the buffer.
		}
	*/

	int fontWidth, fontHeight;
	int fontOffset = 1;
	TTF_SizeText(font, "A", &fontWidth, &fontHeight);
	SDL_Rect textArea;
	textArea.x = 10;
	textArea.y = 60;
	textArea.w = 400;//200;//text area should be initialized based on the number of max characters that can be placed in text field
	textArea.h = fontHeight + 10;//10 is the padding to ensure text area is slightly bigger than actual text

	float xOffset = 0.05f;
	float yOffset = 0.1f;
	t.bounds.x = textArea.x + (int)(textArea.w * xOffset);
	t.bounds.y = textArea.y + (int)(textArea.h * yOffset);
	t.max_len = ((textArea.w - (int)(textArea.w * xOffset)) / fontWidth) - 1;
	printf("max characters: %d\n", t.max_len);
	//should find a font that is mono

	SDL_Rect blinkRect;
	blinkRect.x = t.bounds.x;
	blinkRect.y = t.bounds.y;
	TTF_SizeText(font, "|", &blinkRect.w, &blinkRect.h);


	SDL_StartTextInput();

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
				ui_global_state.mousePos.x = event.button.x;
				ui_global_state.mousePos.y = event.button.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseClicked = true;
				printf("mousePos: (%d, %d)\n", mousePos.x, mousePos.y);
				break;
			case SDL_MOUSEBUTTONUP:
				mouseClicked = false;
				break;
			case SDL_KEYDOWN:
				//report any key presses to the widgets.
				ui_global_state.keyentered = event.key.keysym.sym;
				ui_global_state.keymod = event.key.keysym.mod;
				if (event.key.keysym.sym == SDLK_BACKSPACE)
				{
					if (!t.text.empty())
					{
						textChanged = true;
						char* lastChar = &t.text[t.text.size() - 1];
						int width;
						TTF_SizeText(font, lastChar, &width, NULL);
						blinkRect.x -= width;
						t.text.pop_back();
					}
				}
				break;
			case SDL_TEXTINPUT:
				if (t.text.size() < t.max_len)
				{
				/*	char* p = event.text.text;
					for (int i = 0;p[i] != '\0';++i)
					{
						int width;
						TTF_SizeText(font, event.text.text, &width, NULL);
						blinkRect.x += width;
					}*/
					blinkRect.x += fontWidth;
					t.text += event.text.text;
					textChanged = true;
				}
				break;
			}
		}

		//Text Field//
		SDL_SetTextInputRect(&textArea);
		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		SDL_RenderDrawRect(render, &textArea);
		if (textChanged)
		{
			if (t.surface_ != NULL)
			{
				SDL_FreeSurface(t.surface_);
				t.surface_ = NULL;
			}

			if (t.texture_ != NULL)
			{
				SDL_DestroyTexture(t.texture_);
				t.texture_ = NULL;
			}

			t.surface_ = TTF_RenderText_Blended(font, t.text.c_str(), t.color);
			t.texture_ = SDL_CreateTextureFromSurface(render, t.surface_);
		}

		TTF_SizeText(font, t.text.c_str(), &t.bounds.w, &t.bounds.h);
		if(t.texture_ != NULL)
			SDL_RenderCopy(render, t.texture_, NULL,&t.bounds);

		textChanged = false;

		//blinking update
		if (currentTime - pastTime >= blinkDelay)
		{
			pastTime = currentTime;
			blinkAlpha = (blinkAlpha == minAlpha) ? maxAlpha : minAlpha;
			SDL_SetTextureAlphaMod(blinkingTexture, blinkAlpha);

			//display fps text in title
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}

		SDL_RenderCopy(render, blinkingTexture, NULL, &blinkRect);

		Text2 textStruct;
		textStruct.char_limit = 20;
		textStruct.font = font;
		textStruct.text = "";
		drawTextField(render,7, SDL_Point{ 50,350 }, &textStruct);

	//GUI Code Testing//
	/*	
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

		//1st version
		drawVerticalSlider(render, 3, scrollWheelColor, scrollBarColor, &scrollWheelRect, &scrollBarRect, &oldMousePos);

		//2nd version
		SDL_Rect bounds2;
		bounds2.x = 300;
		bounds2.y = 150;
		bounds2.w = 20;
		bounds2.h = 150;
		initialScrollValue = drawVerticalSlider(render, 4, &bounds2, initialScrollValue);
		
		//draw label
		SDL_Point labelPos{ 200,0 };
		drawLabel(render, labelPos, font, string("Hello World!"));

		SDL_Point labelPos2{ 200,50 };
		drawLabel(render, labelPos2, font, string("Cool TExt"));
		*/

		oldMousePos.x = mousePos.x;
		oldMousePos.y = mousePos.y;
		ui_global_state.oldMousePos.x = mousePos.x;
		ui_global_state.oldMousePos.y = mousePos.y;
		

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

	}

	SDL_StopTextInput();

	SDL_DestroyTexture(textTextureSolid);
	SDL_DestroyTexture(blinkingTexture);
	TTF_CloseFont(font);
	font = NULL;

	return 0;
}