#include "TextureLoader.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

//returns the texture loaded on success
//nullptr otherwise
SDL_Texture* TextureLoader::Load(SDL_Renderer* render, const char* filepath)
{
	//Preferred implementation
	SDL_Texture* texture = IMG_LoadTexture(render, filepath);
	if (texture == NULL)
	{
		fprintf(stderr, "%s could not be loaded: %s\n", filepath, IMG_GetError());
	}

	return texture;

	//OLD IMPLEMENTATION
	//instead of creating a surface (loading onto ram then loading onto vram)
	//create the texture directly using IMG_LoadTexture()
	/*SDL_Surface* surface = IMG_Load(filepath);

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
	return texture;*/
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


