#include "SpriteComponent.h"
#include "../Utility/TextureLoader.h"
#include <SDL2/SDL_rect.h>

SpriteComponent::SpriteComponent(std::string type) :
	Component(type)
{
	texture = NULL;
	flip = SDL_RendererFlip::SDL_FLIP_NONE;
}
SpriteComponent::~SpriteComponent()
{
}

void SpriteComponent::Init()
{

}
void SpriteComponent::Update(float deltaTime)
{

}

//this is an expensive function to call!
//should only call this function if the image is on the screen!
void SpriteComponent::Draw(SDL_Renderer* render)
{
	SDL_Rect bounds;
	bounds.h = height;
	bounds.w = width;
	bounds.x = x;
	bounds.y = y;

	//no frames improvement between the 2 functions
	//SDL_RenderCopy(render, texture, NULL, &bounds);
	SDL_RenderCopyEx(render, texture, NULL, &bounds, 0.0, NULL, flip);
}

//returns false if srcTexture passed in is NULL,true otherwise
//SetTextureAttributes retrieves the texture's width and height in pixels after the
//srcTexture is successfully loaded by TextureLoader::Load() or TextureManager::Load()
//Note: This function must be called first before manipulating this sprite component's properties!
bool SpriteComponent::SetTextureAttributes(SDL_Texture* srcTexture)
{
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
	}

	return true;
}
