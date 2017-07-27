#ifndef TEXTURE_STORE_H
#define TEXTURE_STORE_H
#include "TextureLoader.h"
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



private:
	SDL_Renderer* render;//a pointer to the game window's renderer
	std::unordered_map<std::string, SDL_Texture*> textures;

};

#endif

