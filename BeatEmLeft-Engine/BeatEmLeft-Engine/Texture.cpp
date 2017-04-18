#include "Texture.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

/*
	Wrapper class to SDL_Texture which accomplishes to 
	make SDL_Texture function calls easier to make.
*/

Texture::Texture()
{
}


Texture::~Texture()
{
}

/* 
	Description: Returns true on successful texture load; otherwise false 
	SDL_Renderer* renderer: rendering context to load texture on
	const char* filepath: filepath source texture is located on disk
*/
bool Texture::LoadTexture(SDL_Renderer* renderer, const char* filepath)
{
	//if texture was already loaded, no need to load it again.
	if (texture != NULL)
		return true;
	SDL_Surface* surface = IMG_Load(filepath);
	if (surface == NULL)
	{
		printf("Error %s could not be loaded : %s\n", filepath, IMG_GetError());
		return false;
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	if (texture == NULL)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

/*
	Description: Returns true on successful texture destruction; otherwise false
	Typically will return false when texture=NULL;
*/
bool Texture::FreeTexture()
{
	if (texture == NULL)
		return false;
	SDL_DestroyTexture(texture);
	return true;
}
