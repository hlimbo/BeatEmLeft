#ifndef STATIC_TEXT_STORE_H
#define STATIC_TEXT_STORE_H
#include <SDL2/SDL_render.h>
#include <unordered_map>
#include <string>

typedef struct _TTF_Font TTF_Font;

//class is responsible for holding a map of non-changing rendered Text objects and loading the Text in as SDL_Textures
//For Now: class will hold static texts of the same font size
class StaticTextStore
{
public:
	StaticTextStore(SDL_Renderer* render,std::string fontFilePath,int ptsize);
	~StaticTextStore();

	SDL_Texture* Load(const std::string text,const SDL_Color& textColor);
	bool Free(const std::string text);
	SDL_Texture* Get(const std::string text);

private:
	std::unordered_map<std::string, SDL_Texture*> texts;
	SDL_Renderer* render;//a pointer to the game window's renderer
	TTF_Font* font;
};

#endif

