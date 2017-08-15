#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

struct SDL_Texture;
struct SDL_Surface;
struct SDL_Renderer;

//TODO: move struct to ImageStore
struct Image
{
	Image(SDL_Texture* texture, SDL_Surface* surface);
	~Image();

	SDL_Texture* texture;
	SDL_Surface* surface;
};

namespace TextureLoader
{
	Image* LoadImage(SDL_Renderer* render, const char* filepath);
	bool Free(Image* image);

	SDL_Texture* Load(SDL_Renderer* render, const char* filepath);
	bool Free(SDL_Texture* texture);
}

#endif

