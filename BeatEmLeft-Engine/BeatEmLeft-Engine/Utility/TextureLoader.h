#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

struct SDL_Texture;
struct SDL_Renderer;

namespace TextureLoader
{
	SDL_Texture* Load(SDL_Renderer* render, const char* filepath);
	bool Free(SDL_Texture* texture);
}

#endif

