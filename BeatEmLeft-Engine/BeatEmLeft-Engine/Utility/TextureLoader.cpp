#include "TextureLoader.h"
#include <SDL_image.h>
#include <SDL_render.h>
#include <stdio.h>
#include <string.h>

Image* TextureLoader::LoadImage(SDL_Renderer * render, const char * filepath)
{
	SDL_Surface* surface = IMG_Load(filepath);
	if (surface == NULL)
	{
		fprintf(stderr, "Error could not load: %s\n", filepath);
		return NULL;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
	if (texture == NULL)
	{
		fprintf(stderr, "%s could not be loaded: %s\n", filepath, SDL_GetError());
		SDL_FreeSurface(surface);
		return NULL;
	}

	return new Image(texture, surface);
}

bool TextureLoader::Free(Image* image)
{
	if (image == nullptr)
		return false;
	delete image;
	image = nullptr;
	return true;
}

//returns the texture loaded on success
//nullptr otherwise
SDL_Texture* TextureLoader::Load(SDL_Renderer* render, const char* filepath)
{
	SDL_Texture* texture = IMG_LoadTexture(render, filepath);
	if (texture == NULL)
	{
		fprintf(stderr, "%s could not be loaded: %s\n", filepath, IMG_GetError());
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

Image::Image(SDL_Texture* texture, SDL_Surface* surface)
{
	this->texture = texture;
	this->surface = surface;
}

Image::~Image()
{
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
	}

	if (surface != NULL)
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}
}
