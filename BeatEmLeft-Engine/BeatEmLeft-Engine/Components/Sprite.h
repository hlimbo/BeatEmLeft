#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

struct Image;
struct SpriteSheet;

struct Sprite
{
	Sprite();
	Sprite(Image* image);
	//can be constructed from a SpriteSheet where it points to a region to render
	Sprite(SpriteSheet* sheet, int frameIndex);
	~Sprite();

	Image* image;

	//helper function
	bool SetTextureAttributes(SDL_Texture* srctexture);

	//modify this width and height to see changes in size of image on screen
	int width, height;

	SDL_Point GetPosition() { return SDL_Point{ position.x - offset.x,position.y - offset.y }; }
	
	//pixel coordinates of image
	SDL_Point position;
	//used to set the pivot point of an image
	SDL_Point offset;

	SDL_RendererFlip flip;
	const char* type;

	SDL_Rect bounds;//might need for sprites that get constructed from sprite sheets?
private:
	//source width and height of image after loaded into the component
	int src_w, src_h;

};

#endif

