#include "TextureStore.h"
#include <SDL_render.h>
#include <assert.h>

TextureStore::TextureStore(SDL_Renderer* render)
{
	this->render = render;
}

TextureStore::~TextureStore()
{
	int destroyCount = 0;
	for (auto it = textures.begin();it != textures.end();/*empty*/)
	{
		SDL_Texture* texture = it->second;
		it = textures.erase(it);
		destroyCount = TextureLoader::Free(texture) ? destroyCount + 1 : destroyCount;
	}

	//debug
	printf("Destroyed Textures: %d\n", destroyCount);
}

//returns nullptr if fileName already exists in textureStore or returns nullptr if
//texture to load is nullptr, otherwise return true on success
SDL_Texture* TextureStore::Load(const std::string fileName,const std::string filePath)
{
	//this is possibly an O(n) check to ensure that the same fileName cannot be added more than once
	if (textures.find(fileName) != textures.end())
		return nullptr;

	SDL_Texture* texture = TextureLoader::Load(render, filePath.c_str());
	if (texture == nullptr)
		return nullptr;

	textures.insert(make_pair(fileName, texture));
	return texture;
}

bool TextureStore::Free(const std::string fileName)
{
	SDL_Texture* texture = textures[fileName];
	textures.erase(fileName);
	return TextureLoader::Free(texture);
}

SDL_Texture* TextureStore::Get(const std::string fileName)
{
	return textures.at(fileName);
}

//use only for sprites
void TextureStore::SetAlpha(std::string fileName, Uint8 alpha, SDL_BlendMode blendMode)
{
	SDL_Texture* texture = textures.at(fileName);
	SDL_SetTextureBlendMode(texture, blendMode);
	alpha = (alpha > 255) ? 255 : (alpha < 0) ? 0 : alpha;
	SDL_SetTextureAlphaMod(texture, alpha);
}


