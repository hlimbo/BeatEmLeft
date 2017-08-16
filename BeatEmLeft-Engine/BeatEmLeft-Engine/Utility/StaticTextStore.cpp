#include "StaticTextStore.h"
#include <assert.h>
#include <SDL2/sdl_ttf.h>

using namespace std;

StaticTextStore::StaticTextStore(SDL_Renderer* render,string fontFilePath, int ptsize)
{
	this->render = render;
	font = TTF_OpenFont(fontFilePath.c_str(), ptsize);
	if (font == NULL)
		fprintf(stderr, "Error: %s\n", TTF_GetError());
}

StaticTextStore::StaticTextStore()
{
	render = NULL;
	font = NULL;
}

StaticTextStore::~StaticTextStore()
{
	if (font != NULL)
	{
		TTF_CloseFont(font);
		font = NULL;
	}

	int destroyCount = 0;
	for (auto it = texts.begin();it != texts.end();it = texts.erase(it))
	{
		SDL_Texture* texture = it->second;
		if (texture != NULL)
		{
			SDL_DestroyTexture(texture);
			destroyCount++;
		}
	}

	printf("Destroyed static texts: %d\n", destroyCount);
}

SDL_Texture* StaticTextStore::Load(const std::string text, const SDL_Color& textColor)
{
	if (texts[text] == nullptr)
		texts.erase(text);
	else
		return texts[text];

	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), textColor);
	if (textSurface == NULL)
		fprintf(stderr, "Error: %s\n", TTF_GetError());

	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, textSurface);
	if (texture == NULL)
		fprintf(stderr, "Error: %s\n", SDL_GetError());

	SDL_FreeSurface(textSurface);
	texts.insert(make_pair(text, texture));

	return texture;
}

bool StaticTextStore::Free(const std::string text)
{
	SDL_Texture* texture = texts[text];
	texts.erase(text);
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		return true;
	}
	return false;
}

SDL_Texture* StaticTextStore::Get(const std::string text)
{
	assert(texts[text] != NULL);
	return texts[text];
}

