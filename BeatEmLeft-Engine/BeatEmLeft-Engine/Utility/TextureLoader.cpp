#include "TextureLoader.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <string.h>

//returns the texture loaded on success
//nullptr otherwise
SDL_Texture* TextureLoader::Load(SDL_Renderer* render, const char* filepath)
{
	//Shorter isn't always better..
	//SDL_Texture* texture = IMG_LoadTexture(render, filepath);
	//if (texture == NULL)
	//{
	//	fprintf(stderr, "%s could not be loaded: %s\n", filepath, IMG_GetError());
	//}

	//return texture;

	//OLD WAY (copy surface to texture)
	//SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
	//SDL_FreeSurface(surface);
	//surface = NULL;
	//if (texture == NULL)
	//{
	//	printf("Error: %s\n", SDL_GetError());
	//}

	//creating a surface (loading onto ram then loading onto vram)
	SDL_Surface* surface = IMG_Load(filepath);

	if (surface == NULL)
	{
		printf("Error %s could not be loaded : %s\n", filepath, IMG_GetError());
		return NULL;
	}

	//should I cache my SDL_Surface and do what was advised in:
	//https://discourse.libsdl.org/t/is-it-possible-to-modify-a-part-of-an-sdl-textures-alpha-value/22841/2
	//should I cache my surface struct and modify its alpha values then turn into a texture
	//or should I enable texture streaming from a blank texture and copy over the texture's contents
	//to the blank texture with texture streaming enabled?

	SDL_Surface* formatSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, NULL);
	if (formatSurface == NULL)
	{
		printf("Error: %s\n", SDL_GetError());
		return NULL;
	}

	SDL_Texture* newTexture = SDL_CreateTextureFromSurface(render, formatSurface);
	Uint32 format;
	int access;
	int w;
	int h;
	SDL_QueryTexture(newTexture, &format, &access, &w, &h);
	printf("access: %s\n", SDL_GetPixelFormatName(format));

	//if I want to support alpha blending and/or color modulation, if texture streaming is too slow
	//for my needs I will need to switch over to holding a cache of SDL_Surfaces, where every time
	//a SDL_Surface's alpha values change, I destroy the pre-existing texture and recreate the updated
	//texture from the updated SDL_Surface

	//SDL_Texture* newTexture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888,SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, formatSurface->w, formatSurface->h);
	//if (newTexture == NULL)
	//{
	//	printf("Error: %s\n", SDL_GetError());
	//	return NULL;
	//}

	//void* mPixels;
	//int mPitch;
	//SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);
	////copy image from main memory(RAM) to vram
	//memcpy(mPixels, formatSurface->pixels, formatSurface->pitch * formatSurface->h);
	//SDL_UnlockTexture(newTexture);
	//mPixels = NULL;

	SDL_FreeSurface(formatSurface);
	SDL_FreeSurface(surface);

	return newTexture;
}

//returns true if texture was freed, false otherwise
bool TextureLoader::Free(SDL_Texture* texture)
{
	if (texture == NULL)
		return false;
	SDL_DestroyTexture(texture);
	texture = NULL;
	return true;
}


