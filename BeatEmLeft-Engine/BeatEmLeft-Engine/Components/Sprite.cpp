#include "Sprite.h"
#include "../Utility/TextureLoader.h"
#include "../Components/SpriteSheet.h"
#include <stdio.h>
#include <typeinfo.h>
#include <assert.h>

Sprite::Sprite()
{
	image = NULL;
	flip = SDL_RendererFlip::SDL_FLIP_NONE;
	type = typeid(Sprite).name();
}
Sprite::~Sprite()
{
}

Sprite::Sprite(Image* image)
{
	this->image = image;
	SetTextureAttributes(image->texture);
	flip = SDL_RendererFlip::SDL_FLIP_NONE;
	type = typeid(Sprite).name();

	bounds.x = 0;
	bounds.y = 0;
	bounds.h = src_w;
	bounds.w = src_h;
	width = src_w;
	height = src_h;
}

Sprite::Sprite(SpriteSheet* sheet, int frameIndex)
{
	assert(sheet != nullptr);
	assert(frameIndex < sheet->GetFrameCount() && frameIndex >= 0);
		
	image = sheet->image;
	src_w = sheet->getFrameWidth();
	src_h = sheet->getFrameHeight();
	width = src_w;
	height = src_h;

	//bounds is relative to the original texture's map
	//deep-copy of the sdl rect frame
	bounds.h = sheet->GetFrame(frameIndex)->h;
	bounds.w = sheet->GetFrame(frameIndex)->w;
	bounds.x = sheet->GetFrame(frameIndex)->x;
	bounds.y = sheet->GetFrame(frameIndex)->y;

	flip = SDL_RendererFlip::SDL_FLIP_NONE;
	type = typeid(Sprite).name();
}



//Might turn into a static function since both sprite and spritesheet use this function to load in 
//extra information about the texture that was already loaded in.

//returns false if srcTexture passed in is NULL,true otherwise
//SetTextureAttributes retrieves the texture's width and height in pixels after the
//srcTexture is successfully loaded by TextureLoader::Load() or TextureManager::Load()
//Note: This function must be called first before manipulating this sprite component's properties!
bool Sprite::SetTextureAttributes(SDL_Texture* srcTexture)
{
	if (srcTexture == NULL)
		return false;

	image->texture = srcTexture;
	if (SDL_QueryTexture(image->texture, NULL, NULL, &src_w, &src_h) == -1)
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

