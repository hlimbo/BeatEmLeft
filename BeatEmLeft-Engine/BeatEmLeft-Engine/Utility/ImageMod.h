#ifndef IMAGE_MOD_H
#define IMAGE_MOD_H
#include <SDL2/SDL_render.h>

struct Image;

namespace ImageMod
{
	void SetAlpha(Image* image, Uint8 alpha, SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
	//whenever a portion of a texture's alpha values is requested to be changed.
	//1. Update the texture's cached surface.
	//2. Destroy (Free) the SDL_Texture*
	//3. Create a new SDL_Texture* using the updated cached surface's alpha values.
	//Modifies the selected region's alpha values from a texture
	void SetAlpha(Image* image, Uint8 alpha, SDL_Rect region, SDL_Renderer* render);
	
	//helper functions
	Uint32 GetPixelInfo(void* pixels, int pitch, int Bpp, int x, int y);
	void SetPixelInfo(SDL_Surface* surface, int x, int y, Uint32 pixel);
	SDL_Color GetPixelColor(SDL_Surface* surface, SDL_Point point);
	void SetPixelColor(SDL_Surface* surface, SDL_Point point, SDL_Color color);
}


#endif
