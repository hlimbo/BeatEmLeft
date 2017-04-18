#include "Sprite.h"
#include <SDL2/SDL.h>
#include "Texture.h"
#include <stdio.h>

Sprite::Sprite()
{
	texture = new Texture();
	bounds.x = 0;
	bounds.y = 0;
}

Sprite::~Sprite()
{
	delete texture;
}

Sprite::Sprite(SDL_Point location)
{
	MoveSprite(location);
}

Sprite::Sprite(int x, int y)
{
	MoveSprite(x, y);
}


//Loads the sprite with its default width and height in pixel properties read from the texture file.
bool Sprite::LoadSprite(SDL_Renderer * renderer, const char * filepath)
{
	if (texture->LoadTexture(renderer, filepath))
	{
		int status = SDL_QueryTexture(texture->GetSource(), NULL, NULL, &bounds.w, &bounds.h);
		if (status == 0)
			return true;
	}

	return false;
}
/* 
	True on successful draw; false otherwise
	Call this function after loading or moving sprite to display sprite on screen
*/
//bool Sprite::DrawSprite(SDL_Renderer* renderer,SDL_Point location, int width, int height)
bool Sprite::DrawSprite(SDL_Renderer* renderer)
{
	if (SDL_RenderCopy(renderer, texture->GetSource(), NULL, &bounds) == -1)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_RenderPresent(renderer);	
	return true;
}

void Sprite::MoveSprite(int dx, int dy)
{
	bounds.x += dx;
	bounds.y += dy;
}

void Sprite::MoveSprite(SDL_Point dPosition)
{
	bounds.x += dPosition.x;
	bounds.y += dPosition.y;
}

void Sprite::SetLocation(int x, int y)
{
	bounds.x = x;
	bounds.y = y;
}

void Sprite::SetLocation(SDL_Point newLocation)
{
	bounds.x = newLocation.x;
	bounds.y = newLocation.y;
}

int Sprite::GetHeight()
{
	return bounds.h;
}

int Sprite::GetWidth()
{
	return bounds.w;
}

SDL_Point Sprite::GetLocation()
{
	return SDL_Point{ bounds.x , bounds.y };
}

/*
	sizeInPixels.x = width in pixels
	sizeInPixels.y = height in pixels
*/
void Sprite::SetSize(SDL_Point sizeInPixels)
{
	bounds.w = sizeInPixels.x;
	bounds.h = sizeInPixels.y;
}
void Sprite::SetSize(int w, int h)
{
	bounds.w = w;
	bounds.h = h;
}

bool Sprite::FreeSprite()
{
	return texture->FreeTexture();
}
