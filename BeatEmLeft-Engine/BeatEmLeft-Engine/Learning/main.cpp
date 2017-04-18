#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>

/*
errorDesc: describes the context of the error i.e. SDL_InitError
errorMsg: returns the error msg retrieved from SDL_GetError();
isActive: used to turn on/off error messages useful for debugging.
*/
void Error(const char* errorDesc, const char* errorMsg, bool isActive = true);
/* sample utility function from http://www.willusher.io/sdl2%20tutorials/2014/06/16/postscript-0-properly-finding-resource-paths*/
std::string getResourcePath(const std::string &subDir = "resources");

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		Error("SDL_Init Error: ", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	int width = 640;
	int height = 480;
	SDL_Window* window = SDL_CreateWindow("BeatEmLeft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,SDL_WINDOW_SHOWN);
	if (!window)
	{
		Error("SDL_CreateWindow Error: ", SDL_GetError());
	}

	SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!render)
	{
		SDL_DestroyWindow(window);
		Error("SDL_CreateRenderer Error: ", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	//Load the bmp image file.
	std::string imagePath = getResourcePath() + "white.bmp";
	SDL_Surface* bmp = SDL_LoadBMP(imagePath.c_str());
	if (!bmp)
	{
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		Error("SDL_LoadBMP Error: ", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	//convert image file to texture.
	SDL_Texture*  texture = SDL_CreateTextureFromSurface(render, bmp);
	SDL_FreeSurface(bmp);
	if (!texture)
	{
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		Error("SDL_CreateTextureFromSurface Error: ", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	int numTimesRender = 1;
	for (int i = 0;i < numTimesRender; ++i)
	{
		//Draw the Texture to screen
		//clear the renderer
		if (SDL_RenderClear(render) != 0)
		{
			Error("SDL_RenderClear(): ", SDL_GetError());
			SDL_DestroyRenderer(render);
			SDL_DestroyWindow(window);
			SDL_Quit();
			return 1;
		}

		//draw a part of the texture. ~ useful for tile splitting later
		SDL_Rect srcRect;
		srcRect.h = 250;
		srcRect.w = 400;
		srcRect.x = 400;
		srcRect.y = 0;

		//draw the texture to fit the given rectangle
		SDL_Rect dstRect;
		dstRect.h = 50;
		dstRect.w = 50;
		dstRect.x = width / 2;
		dstRect.y = height / 2;
		//render the texture to screen
		if (SDL_RenderCopy(render, texture, NULL, NULL) != 0)
		{
			Error("SDL_RenderCopy(): ", SDL_GetError());
			SDL_DestroyRenderer(render);
			SDL_DestroyWindow(window);
			SDL_Quit();
			return 1;
		}

		//present the updated screen
		SDL_RenderPresent(render);

		//sleep for 1000 ms
		SDL_Delay(1000);
	}

	//Draw png, jpg img
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted&flags) != flags)
	{
		Error("IMG_Init: Failed to init required png and jpg support: ", IMG_GetError());
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		SDL_Quit();
		IMG_Quit();
		return 1;
	}


	//load a png image.
	SDL_Surface* image;
	std::string imagePath2 = getResourcePath() + "block.png";
	image = IMG_Load(imagePath2.c_str());
	if (!image)
	{
		Error("IMG_Load: ", IMG_GetError());
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		SDL_Quit();
		IMG_Quit();
		return 1;
	}

	//create texture from the surface loaded from file.
	SDL_Texture* img_text = SDL_CreateTextureFromSurface(render, image);
	if (!img_text)
	{
		Error("SDL_CreateTextureFromSurface: ", IMG_GetError());
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		SDL_Quit();
		IMG_Quit();
		return 1;
	}

	int clear = SDL_RenderClear(render);
	int rc = SDL_RenderCopy(render, img_text, NULL, NULL);
	SDL_RenderPresent(render);

	SDL_Delay(5000);
	SDL_Quit();
	return 0;
}

void Error(const char* errorDesc, const char* errorMsg, bool isActive)
{
	if (isActive)
		std::cerr << errorDesc << errorMsg << std::endl;
}
std::string getResourcePath(const std::string &subDir)
{
//cross platform compatability for / differences
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif

	static std::string baseRes;
	char* basePath = SDL_GetBasePath();
	if (basePath)
	{
		baseRes = basePath;
		baseRes += subDir + PATH_SEP;
		SDL_free(basePath);
	}
	else
	{
		Error("Error getting resource path: ", SDL_GetError());
		return "";
	}

	return baseRes;
}