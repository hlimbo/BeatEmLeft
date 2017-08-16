#ifndef GUI_H
#define GUI_H
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>

struct SDL_Renderer;
struct SDL_Texture;

//Immediate Mode GUI functions used to draw gui widgets
namespace GUI
{
	//Should not declare more than one ui_state since this holds
	//a global state to all widgets being rendered on the screen
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
		//retrieves text input from keyboard when SDL_TEXTINPUT event is triggered
		char* textBuffer;
		//contains all textures of all rendered text displayed on screen
		//where the key = ui_id,value = SDL_Texture*
		std::unordered_map<int, SDL_Texture*> textBufferTextures;

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

		~ui_state() 
		{
			for (auto it = textBufferTextures.begin();
				it != textBufferTextures.end();
				it = textBufferTextures.erase(it))
			{
				SDL_Texture* texture = it->second;
				if (texture != NULL)
				{
					SDL_DestroyTexture(texture);
					texture = NULL;
				}
			}
		}
	};

	//state handles all changes that happen to every widget
	extern ui_state ui_global_state;
	
	void ProcessEvent(SDL_Event* event);

	//Note:: the following functions are all drawn relative to the application's main window

	//returns the position of the knob as a floating point value mapped between a min and max float value.
	//Note: initialValue must be a floating point value between 0.0f and 1.0f inclusive
	float VerticalSlider(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, float value, const SDL_Color& color);
	float HorizontalSlider(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, float value,const SDL_Color& color);

	//returns true if toggle is marked, otherwise it returns false
	bool Toggle(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, bool isToggled, const SDL_Color& color);

	//**Note** Need to figure out a way to efficiently render static text to the screen!
	//returns true if button is pressed, otherwise it returns false
	bool Button(SDL_Renderer* render, int ui_id, const SDL_Rect* bounds, const SDL_Color& color,const std::string text,TTF_Font* font);

	//constructs a text field where its size is proportional to Text::char_limit
	//returns the updated text typed into the text field
	std::string TextField(SDL_Renderer* render, int ui_id, const SDL_Rect* textBoxRect,std::string text, const SDL_Color& color,TTF_Font* font);
}


#endif
