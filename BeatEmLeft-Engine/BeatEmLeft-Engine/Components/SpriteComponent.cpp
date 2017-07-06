#include "SpriteComponent.h"
#include "../Utility/TextureLoader.h"
#include <SDL2/SDL_render.h>//

SpriteComponent::SpriteComponent(std::string type) :
	Component(type)
{
	texture = NULL;
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
bool SpriteComponent::LoadTexture(SDL_Renderer* render, std::string filePath)
{
	return TextureLoader::Load(render, filePath.c_str());
}
