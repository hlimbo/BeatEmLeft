#include "GUI.h"
#include <SDL2/SDL.h>
#include <assert.h>

GUI::ui_state GUI::ui_global_state;

using namespace std;

void GUI::ProcessEvent(SDL_Event* event)
{
	switch (event->type)
	{
	case SDL_MOUSEMOTION:
		ui_global_state.mousePos.x = event->button.x;
		ui_global_state.mousePos.y = event->button.y;
		break;
	case SDL_MOUSEBUTTONDOWN:
		printf("MousePos: (%d,%d)\n", ui_global_state.mousePos.x, ui_global_state.mousePos.y);
		ui_global_state.mouseClicked = true;
		break;
	case SDL_MOUSEBUTTONUP:
		ui_global_state.mouseClicked = false;
		break;
	case SDL_KEYDOWN:
		ui_global_state.keyPressed = event->key.keysym.sym;
		ui_global_state.keyMod = event->key.keysym.mod;
		if (ui_global_state.keyPressed == SDLK_BACKSPACE)
			ui_global_state.textChanged = true;
		break;
	case SDL_KEYUP:
		ui_global_state.keyPressed = -1;
		ui_global_state.keyMod = 0;
		break;
	case SDL_TEXTINPUT:
		ui_global_state.textBuffer = event->text.text;
		ui_global_state.textChanged = true;
		break;
	}
}

float GUI::VerticalSlider(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, float initialValue, const SDL_Color& color)
{
	//construct the knob bounds
	float knobScale = 0.25f;
	SDL_Rect knobBounds;
	knobBounds.x = bounds->x;
	knobBounds.w = bounds->w;
	knobBounds.h = (int)(knobScale * bounds->h);

	SDL_Color knobColor{ color.r, color.g, color.b, color.a };
	Uint8 d = (Uint8)4;
	SDL_Color barColor{ color.r / d, color.g / d, color.b / d, color.a };

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

	//set pressedID = 0 when mouse button is released
	if (ui_global_state.pressedID == ui_id && !mouseClicked)
		ui_global_state.pressedID = 0;

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
		//ui_global_state.pressedID = 0;
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

float GUI::HorizontalSlider(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, float initialValue, const SDL_Color& color)
{
	//construct the knob bounds
	float knobScale = 0.25f;
	SDL_Rect knobBounds;
	knobBounds.x = bounds->x;
	knobBounds.y = bounds->y;
	knobBounds.w = (int)(knobScale * bounds->w);
	knobBounds.h = bounds->h;

	SDL_Color knobColor;
	knobColor.r = color.r / 2;
	knobColor.g = color.g / 2;
	knobColor.b = color.b / 2;
	knobColor.a = color.a / 2;

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

	//set pressedID = 0 when mouse button is released
	if (ui_global_state.pressedID == ui_id && !mouseClicked)
		ui_global_state.pressedID = 0;

	//keep the scroll knob in place anywhere that it is clicked on
	//move the scroll knob by the amount the mouse position changes (delta mouse pos)

	float minValue = 0.0f;
	float maxValue = 1.0f;
	float value = (initialValue < minValue) ? minValue : (initialValue > maxValue) ? maxValue : initialValue;
	knobBounds.x = (int)(value * bounds->w) + bounds->x;
	if (ui_global_state.pressedID == ui_id)
	{
		int deltaMousePos = newMousePos.x - ui_global_state.oldMousePos.x;
		if (deltaMousePos != 0)
		{
			//deltaV ranges from -1 to 1 inclusive
			float deltaV = (float)deltaMousePos / bounds->w;
			value += deltaV;
			value = (value > maxValue) ? maxValue : (value < minValue) ? minValue : value;
		}
		//printf("value: %f\n", value);	
		knobBounds.x = (int)(value * bounds->w) + bounds->x;
	}

	//since the knob has a height and its coordinates are measured from its top-left corner, 
	//its necessary to make sure the knob doesn't move past its scrollable area.
	if (knobBounds.x + knobBounds.w > bounds->x + bounds->w)
		knobBounds.x = (bounds->x + bounds->w) - knobBounds.w;

	//rendering
	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
	//draw scroll bar
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(render, bounds);

	//draw scroll knob
	if (ui_global_state.pressedID == ui_id)
	{
		//ui_global_state.pressedID = 0;
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

bool GUI::Toggle(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, bool isToggled, const SDL_Color& color)
{
	//construct check-mark bounds
	SDL_Rect checkmarkBounds;
	checkmarkBounds.x = bounds->x + bounds->w / 4;
	checkmarkBounds.y = bounds->y + bounds->h / 4;
	checkmarkBounds.w = bounds->w - bounds->w / 2;
	checkmarkBounds.h = bounds->h - bounds->h / 2;

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

	if (ui_global_state.pressedID == ui_id && ui_global_state.hoveredID == ui_id && !mousePressed)
	{
		ui_global_state.pressedID = 0;
		isToggled = !isToggled;
	}

	//render toggle
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
	if (isToggled)
		SDL_RenderFillRect(render, &checkmarkBounds);
	SDL_RenderDrawRect(render, bounds);

	return isToggled;
}

bool GUI::Button(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, const SDL_Color& color, SDL_Texture* text)
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
		ui_global_state.hoveredID = 0;
		SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(render, bounds);
		SDL_SetRenderDrawColor(render, color.r / 2, color.g / 2, color.b / 2, color.a / 2);
		SDL_RenderFillRect(render, bounds);
	}
	else //inactive
	{
		SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(render, bounds);
	}

	//render text ~ temporary placeholder text
	SDL_Rect textArea;
	SDL_QueryTexture(text, NULL, NULL, &textArea.w, &textArea.h);
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

	if (ui_global_state.keyboardFocusID == ui_id && ui_global_state.keyPressed == -1 && ui_global_state.prevPressed == SDLK_RETURN)
	{
		ui_global_state.prevPressed = -1;
		return true;
	}

	//button was not pressed
	return false;
}

string GUI::TextField(SDL_Renderer* render, int ui_id, const SDL_Rect* textBoxRect, std::string text, const SDL_Color& color,TTF_Font* font)
{
	//check if text field is hovered over or clicked on
	SDL_Point mousePos;
	bool mousePressed = SDL_GetMouseState(&mousePos.x, &mousePos.y) & SDL_BUTTON(SDL_BUTTON_LEFT);
	if (SDL_PointInRect(&mousePos, textBoxRect))
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
		else if (ui_global_state.keyboardFocusID == ui_id)
			ui_global_state.keyboardFocusID = 0;
	}

	//construct text area
	int fontHeight, fontWidth;
	TTF_SizeUTF8(font, "A", &fontWidth, &fontHeight);
	SDL_Rect textAreaRect;
	textAreaRect.x = textBoxRect->x + (fontWidth / 2);
	textAreaRect.y = textBoxRect->y + (fontHeight / 4);

	int char_limit = (textBoxRect->w - (fontWidth / 2)) / fontWidth;

	if (ui_global_state.keyboardFocusID == 0)
	{
		SDL_StopTextInput();
	}
	else if (ui_global_state.keyboardFocusID == ui_id)
	{
		ui_global_state.pressedID = 0;
		SDL_StartTextInput();

		//check if paste shortcut key pressed (ctrl-v)
		//adds on to end of text
		if (ui_global_state.keyPressed == SDLK_v && (ui_global_state.keyMod & KMOD_CTRL))
		{
			ui_global_state.keyPressed = -1;
			string temp(SDL_GetClipboardText());
			if (temp.size() + text.size() < char_limit)
			{
				text += temp;
				ui_global_state.textChanged = true;
			}
		}

		if (ui_global_state.textChanged)
		{
			ui_global_state.textChanged = false;
			if (ui_global_state.keyPressed == SDLK_BACKSPACE)
			{
				if (!text.empty())
					text.pop_back();
			}
			else // text input
			{
				if (char_limit > text.size() && ui_global_state.textBuffer != NULL)
				{
					text += string(ui_global_state.textBuffer);
					ui_global_state.textBuffer = NULL;
				}
			}

			if (ui_global_state.textBufferTextures[ui_id] != NULL)
			{
				SDL_DestroyTexture(ui_global_state.textBufferTextures[ui_id]);
				ui_global_state.textBufferTextures[ui_id] = NULL;
			}

			SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
			ui_global_state.textBufferTextures[ui_id] = SDL_CreateTextureFromSurface(render, textSurface);
			SDL_FreeSurface(textSurface);
		}
		else
		{
			//check if copy shortcut key pressed (ctrl-c)
			if (ui_global_state.keyPressed == SDLK_c && (ui_global_state.keyMod & KMOD_CTRL))
			{
				ui_global_state.keyPressed = -1;
				SDL_SetClipboardText(text.c_str());
			}
		}

		//construct text cursor
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
			SDL_Rect textCursorRect;
			TTF_SizeText(font, text.c_str(), &textAreaRect.w, NULL);
			textCursorRect.x = textAreaRect.x + textAreaRect.w;
			TTF_SizeText(font, "|", &textCursorRect.w, &textCursorRect.h);
			textCursorRect.y = textAreaRect.y - (fontHeight / 8);

			SDL_Surface* textCursor_s = TTF_RenderText_Blended(font, "|", color);
			SDL_Texture* textCursor_t = SDL_CreateTextureFromSurface(render, textCursor_s);
			SDL_RenderCopy(render, textCursor_t, NULL, &textCursorRect);
			SDL_FreeSurface(textCursor_s);
			SDL_DestroyTexture(textCursor_t);
		}
	}

	//display textbox and text
	SDL_SetRenderDrawColor(render, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(render, textBoxRect);

	SDL_Texture* texture = ui_global_state.textBufferTextures[ui_id];
	if (texture == NULL)
	{
		if(text.empty())
			ui_global_state.textBufferTextures.erase(ui_id);
		else
		{
			SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);
			ui_global_state.textBufferTextures[ui_id] = SDL_CreateTextureFromSurface(render, textSurface);
			SDL_FreeSurface(textSurface);
		}
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &textAreaRect.w, &textAreaRect.h);
		SDL_RenderCopy(render, texture, NULL, &textAreaRect);
	}

	return text;
}

//string text("Sample Text");
//SDL_Color blue{0,0,255,255};
//SDL_Rect textArea{10,320,100,TTF_FontHeight(font)};
//TextField(render,__LINE__,textArea,text,blue);