#include "Sprite.h"
#include <stdio.h>
#include <typeinfo.h>

Sprite::Sprite()
{
	texture = NULL;
	flip = SDL_RendererFlip::SDL_FLIP_NONE;
	type = typeid(Sprite).name();
}
Sprite::~Sprite()
{
}

Sprite::Sprite(SDL_Texture* srcTexture)
{
	texture = NULL;
	SetTextureAttributes(srcTexture);
	flip = SDL_RendererFlip::SDL_FLIP_NONE;
	type = typeid(Sprite).name();
}

//returns false if srcTexture passed in is NULL,true otherwise
//SetTextureAttributes retrieves the texture's width and height in pixels after the
//srcTexture is successfully loaded by TextureLoader::Load() or TextureManager::Load()
//Note: This function must be called first before manipulating this sprite component's properties!
bool Sprite::SetTextureAttributes(SDL_Texture* srcTexture)
{
	if (texture != NULL)
		return false;
	if (srcTexture == NULL)
		return false;

	texture = srcTexture;
	if (SDL_QueryTexture(texture, NULL, NULL, &src_w, &src_h) == -1)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		width = src_w;
		height = src_h;
		position.x = 0;
		position.y = 0;
	}

	return true;
}
