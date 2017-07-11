#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <SDL2/SDL_render.h>

struct SDL_Texture;
struct SDL_Renderer;

struct Sprite
{
	Sprite();
	Sprite(SDL_Texture* srcTexture);
	~Sprite();

	//helper function
	SDL_Texture* texture;
	bool SetTextureAttributes(SDL_Texture* srcTexture);

	//modify this width and height to see changes in size of image on screen
	int width, height;
	//pixel coordinates of image
	int x, y;
	//flipping the image
	SDL_RendererFlip flip;
	const char* type;
private:
	//source width and height of image after loaded into the component
	int src_w, src_h;

};

#endif

