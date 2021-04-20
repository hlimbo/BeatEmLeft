#ifndef IMAGE_STORE_H
#define IMAGE_STORE_H
#include "TextureLoader.h"
#include <SDL_render.h>
#include <unordered_map>
#include <string>

class ImageStore
{
public:
	ImageStore(SDL_Renderer* render);
	~ImageStore();

	Image* Load(const std::string fileName, const std::string filePath);
	bool Free(const std::string fileName);
	Image* Get(const std::string fileName);

	//Note: alpha is clamped between 0-255 inclusive
	void SetAlpha(const std::string fileName, Uint8 alpha, SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND);
	void SetAlpha(const std::string fileName, Uint8 alpha, SDL_Rect region);
private:
	SDL_Renderer* render;//a pointer to the game window's renderer
	std::unordered_map<std::string, Image*> images;
};

#endif

