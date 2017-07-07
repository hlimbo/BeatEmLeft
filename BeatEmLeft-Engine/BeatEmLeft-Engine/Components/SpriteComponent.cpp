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
	TextureLoader::Free(texture);
}

void SpriteComponent::Init()
{

}
void SpriteComponent::Update(float deltaTime)
{

}
void SpriteComponent::Draw(SDL_Renderer * render)
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
bool SpriteComponent::LoadTexture(SDL_Renderer* render, std::string filePath)
{
	texture = TextureLoader::Load(render, filePath.c_str());
	if (texture == NULL)
		return false;

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
