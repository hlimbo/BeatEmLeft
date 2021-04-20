#ifndef GUI_H
#define GUI_H
#include <SDL_rect.h>
#include <SDL_pixels.h>
#include <SDL_events.h>
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;
struct SpriteSheet;

//Immediate Mode GUI functions used to draw gui widgets

/*
	Immediate Mode GUI is a procedure based way of creating gui
	widgets visible in the screen. Benefits of this approach is that
	it is relatively easy to hook gui widgets into the application level
	code (e.g. where the main function is located) and reason about the
	underlying logic of the code.

	ui_global_state maintains the state of all widgets placed in the application.

*/

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
		bool textEditingEnabled;
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

		//pointer to main window's renderer
		SDL_Renderer* render;

		ui_state()
		{
			hoveredID = 0;
			pressedID = 0;
			mousePos = SDL_Point{ 0,0 };
			oldMousePos = SDL_Point{ 0,0 };
			mouseClicked = false;
			textChanged = false;
			textEditingEnabled = false;
			keyboardFocusID = 0;
			keyPressed = -1;
			prevPressed = -1;
			currentTime = 0.0f;
			pastTime = 0.0f;
			isTextCursorVisible = true;
			textBuffer = NULL;
			render = NULL;
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
	
	void Init(SDL_Renderer* render);
	void SetTimeAndOldMousePos(const float& currentTime);
	void ProcessEvent(SDL_Event* event);

	//Note:: the following functions are all drawn relative to the application's main window

	//returns the position of the knob as a floating point value mapped between a min and max float value.
	//Note: initialValue must be a floating point value between 0.0f and 1.0f inclusive
	float VerticalSlider(int ui_id, const SDL_Rect* bounds, float value, const SDL_Color& color);
	float HorizontalSlider(int ui_id, const SDL_Rect* bounds, float value,const SDL_Color& color);

	//returns true if toggle is marked, otherwise it returns false
	bool Toggle(int ui_id, const SDL_Rect* bounds, bool isToggled, const SDL_Color& color);

	//returns true if button is pressed, otherwise it returns false
	bool Button(int ui_id, const SDL_Rect* bounds, const SDL_Color& color,const std::string text,TTF_Font* font);

	//constructs a text field where its size is proportional to Text::char_limit
	//returns the updated text typed into the text field
	std::string TextField(int ui_id, const SDL_Rect* textBoxRect,std::string text, const SDL_Color& color,TTF_Font* font);

	void Label(int ui_id,const SDL_Point* screen_pos, TTF_Font* font, const std::string& text,const SDL_Color& color);

	int GridSelector(int ui_id, const SDL_Rect* bounds, SpriteSheet* sheet,int xAcross);

	/*  Summary:
			bool(*window_func)(int, const SDL_Rect*, TTF_Font*) is a function pointer where
			the user can program additional GUI elements inside of the function they define for the window to display
		Parameters:
			int = ui_id
			const SDL_Rect* = position all gui elements contained inside the window is relative to (this is the window's position)
			TTF_Font* = todo.. will remove later on but is needed for GUI Elements that need to display text
		Return Value:
			the function returns true if a GUI element was pressed inside the window, false otherwise
	*/
	SDL_Rect Window(int ui_id, const SDL_Rect* bounds, TTF_Font* font, bool(*window_func)(int, const SDL_Rect*, TTF_Font*), SDL_Color windowcolor = {60,87,121,255});

	int Toolbar(int ui_id,const SDL_Rect* bounds, int toolbarIndex, const std::vector<std::string> textList,TTF_Font* font);
}


#endif
