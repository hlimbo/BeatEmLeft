#ifndef TEXTURE_H
#define TEXTURE_H

struct SDL_Texture;
struct SDL_Renderer;

class Texture
{
public:
	Texture();
	~Texture();
	bool LoadTexture(SDL_Renderer* renderer, const char* filepath);
	bool FreeTexture();
	SDL_Texture* GetSource();
private:
	SDL_Texture* texture;
};

#endif

