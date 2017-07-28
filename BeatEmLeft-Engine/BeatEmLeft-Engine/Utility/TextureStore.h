#ifndef TEXTURE_STORE_H
#define TEXTURE_STORE_H
#include "TextureLoader.h"
#include <SDL2/SDL_render.h>
#include <unordered_map>
#include <string>

struct SDL_Texture;
struct SDL_Renderer;

class TextureStore
{
public:
	TextureStore(SDL_Renderer* render);
	~TextureStore();

	SDL_Texture* Load(const std::string fileName,const std::string filePath);
	bool Free(const std::string fileName);
	SDL_Texture* Get(const std::string fileName);

	//Note: alpha is clamped between 0-255 inclusive
	void SetAlpha(const std::string fileName, Uint8 alpha, SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);

private:
	SDL_Renderer* render;//a pointer to the game window's renderer
	std::unordered_map<std::string, SDL_Texture*> textures;
};

#endif

