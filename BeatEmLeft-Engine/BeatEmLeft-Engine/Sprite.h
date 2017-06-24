#ifndef SPRITE_H
#define SPRITE_H
#include <SDL2/SDL_rect.h>

class Texture;
struct SDL_Renderer;

//sprite's pivot point is its top left corner
class Sprite
{
public:
	Sprite();
	~Sprite();

	Sprite(SDL_Point location);
	Sprite(int x, int y);

	bool LoadSprite(SDL_Renderer* renderer, const char* filepath);
	bool FreeSprite();
	//bool DrawSprite(SDL_Renderer* renderer, SDL_Point location, int width, int height);
	bool DrawSprite(SDL_Renderer* renderer);
	void MoveSprite(int dx, int dy);
	void MoveSprite(SDL_Point dPosition);
	void SetLocation(int x, int y);
	void SetLocation(SDL_Point newLocation);
	void SetSize(SDL_Point sizeInPixels);
	void SetSize(int w, int h);

	int GetHeight();
	int GetWidth();
	SDL_Point GetLocation();
public:
	SDL_Point scale;
private:
	Texture* texture;
	//holds the width, height, and (x,y) coordinates of sprite
	SDL_Rect bounds;
};

#endif

