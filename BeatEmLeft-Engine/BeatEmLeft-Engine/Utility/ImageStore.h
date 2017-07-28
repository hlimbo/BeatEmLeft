#ifndef IMAGE_STORE_H
#define IMAGE_STORE_H
#include "TextureLoader.h"
#include <SDL2/SDL_render.h>
#include <unordered_map>
#include <string>

struct SDL_Texture;
struct SDL_Renderer;

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

	//whenever a portion of a texture's alpha values is requested to be changed.
	//1. Update the texture's cached surface.
	//2. Destroy (Free) the SDL_Texture*
	//3. Create a new SDL_Texture* using the updated cached surface's alpha values.
	//Modifies the selected region's alpha values from a texture
	void SetAlpha(const std::string fileName, Uint8 alpha, SDL_Rect region);

private:
	SDL_Renderer* render;//a pointer to the game window's renderer
	std::unordered_map<std::string, Image*> images;

	//Helper functions used to modify alpha values of a surface.
	//retrieves the raw pixel information in hexadecimal format
	Uint32 GetPixelInfo(void* pixels, int pitch, int Bpp, int x, int y);
	void SetPixelInfo(SDL_Surface* surface, int x, int y, Uint32 pixel);
	SDL_Color GetPixelColor(SDL_Surface* surface, SDL_Point point);
	void SetPixelColor(SDL_Surface* surface, SDL_Point point, SDL_Color color);
};

#endif

