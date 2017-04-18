#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_assert.h>
#include <string>

#define WIDTH 640
#define HEIGHT 480

//16x16 tiles
//40 tiles wide
//30 tiles high
void Error(const char* errorDesc, const char* errorMsg ="", bool isActive=true);
SDL_Texture* LoadTexture(SDL_Renderer* renderer,std::string path);

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Images", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	//part of initialization
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	IMG_Init(flags);

	std::string baseRes(SDL_GetBasePath());
	baseRes += "\\resources\\";
	std::string bg_path = baseRes + std::string("block.jpg");
	SDL_Texture* bg_texture = LoadTexture(render, bg_path);
	
	SDL_RenderClear(render);
	SDL_RenderCopy(render, bg_texture, NULL, NULL);
	SDL_RenderPresent(render);

	std::string png_path = baseRes + "block.png";
	SDL_Texture* png_image = LoadTexture(render, png_path);

	//2. place a copy of the image on the back screen
	SDL_Rect dstRect;
	//dstRect.x = WIDTH / 2;
	//dstRect.y = HEIGHT / 2;
	//dstRect.w = 50;
	//dstRect.h = 50;
	//SDL_RenderCopy(render, png_image, NULL, &dstRect);
	////3 place copy of image to front screen.
	//SDL_RenderPresent(render);

	int height = 30;
	int width = 40;
	int tiledim = 16;
	dstRect.w = 15;
	dstRect.h = 15;

	//this draws the tile 1 by 1 
	for (int h = 0;h < height;++h)
	{
		for (int w = 0;w < width;++w)
		{
			dstRect.x = w * tiledim;
			dstRect.y = h * tiledim;
			SDL_RenderCopy(render, png_image, NULL, &dstRect);
		}
	}

	SDL_RenderPresent(render);

	SDL_Delay(5000);
	IMG_Quit();
	SDL_Quit();
	return 0;
}

void Error(const char* errorDesc, const char* errorMsg, bool isActive)
{
	if (isActive)
		std::cerr << errorDesc << errorMsg << std::endl;
}

//loads png or jpg files
SDL_Texture* LoadTexture(SDL_Renderer* renderer,std::string path)
{
	//error handling
	if(!renderer)
	{
		Error("LoadTexture(): renderer is null");
		SDL_assert(renderer != NULL);
		return NULL;
	}

	SDL_Surface* surface = IMG_Load(path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);	
	/*safe to call when surface is null*/
	SDL_FreeSurface(surface);
	
	if (!surface)
	{
		Error("LoadTexture(): surface is null");
		SDL_assert(surface != NULL);
		return NULL;
	}

	if (!texture)
	{
		Error("LoadTexture(): texture is null");
		SDL_assert(texture != NULL);
		return NULL;
	}

	return texture;
}