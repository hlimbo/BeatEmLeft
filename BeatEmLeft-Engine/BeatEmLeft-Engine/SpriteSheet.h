#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H
#include <SDL2/SDL_rect.h>

class Texture;
struct SDL_Renderer;

//note use SDL_Rect srcRect to capture the rendering area I want to display on screen.
class SpriteSheet
{
public:
	SpriteSheet();
	~SpriteSheet();

	bool LoadSpriteSheet(SDL_Renderer* renderer, const char* filepath);
	bool FreeSpriteSheet();
	
	bool DrawFrame(SDL_Renderer* renderer,int frameIndex = 0);

	SDL_Point GetSliceDimensions();
	int SliceWidth, SliceHeight;//size of one frame x = width y = height

	int GetFramesLength();

private:
	Texture* texture;
	int framesLength;//how many frames in total sprite sheet contain
	SDL_Rect bounds;

};

#endif