#include "TextureLoader.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

//returns the texture loaded on success
//nullptr otherwise
SDL_Texture* TextureLoader::Load(SDL_Renderer* render, const char* filepath)
{
	SDL_Surface* surface = IMG_Load(filepath);

	if (surface == NULL)
	{
		printf("Error %s could not be loaded : %s\n", filepath, IMG_GetError());
		return NULL;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_FreeSurface(surface);
	surface = NULL;
	if (texture == NULL)
	{
		printf("Error: %s\n", SDL_GetError());
	}

	return texture;
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


