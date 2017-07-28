#include <stdio.h>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Core.h"
#include "Components/SpriteSheet.h"

#include "Systems/RenderSystem.h"
#include "Systems/ComponentManager.h"
#include "Systems/EntitySystem.h"
#include "Systems/ECS.h"

#include "Input/KeyboardController.h"
#include "Utility/TextureStore.h"

using namespace std;

//Utilizes SDL_Surfaces (which are images kept in main memory RAM where the CPU processes the pixel data).
//Modify the alpha values of the pixel data on the CPU.

//retrieves the raw pixel information in hexadecimal format
Uint32 GetPixelInfo(void* pixels, int pitch, int Bpp, int x, int y)
{
	//Note: image width = pitch / BytesPerPixel;

	//inside the parenthesis the pixels are 8-bit addressable (Note: can only use pitch and Bpp when the pixels are 8-bit addressable only)
	//outside the parenthesis the pixels become 32-bit addressable (byte addressable)
	return *(Uint32*)((Uint8*)(pixels) + (y * pitch) + (x * Bpp));
}

//returns the color value in rgba format
SDL_Color GetPixelColor(SDL_Surface* surface, SDL_Point point)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	int Bpp = surface->format->BytesPerPixel;
	int pitch = surface->pitch;
	Uint32 pixelInfo = GetPixelInfo(surface->pixels, pitch, Bpp, point.x, point.y);

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);

	SDL_Color color;
	SDL_GetRGBA(pixelInfo, surface->format, &color.r, &color.g, &color.b, &color.a);

	return color;
}

void SetPixelInfo(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	Uint32* pixels32 = (Uint32*)((Uint8*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel));
	*pixels32 = pixel;

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

void SetPixelColor(SDL_Surface* surface, SDL_Point point, SDL_Color color)
{
	Uint32 newPixelValue = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);	
	SetPixelInfo(surface, point.x, point.y, newPixelValue);
}

int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");
	string objectsPath = mainPath + string("objects.png");
	string redBlockPath = mainPath + string("redblock.png");

	//Note: I don't need to convert pixel formats the conversion from surface to texture is pixel format independent
	SDL_Surface* surface = IMG_Load(objectsPath.c_str());

	int surfaceHeight = surface->h;
	int surfaceWidth = surface->w;
	int mPitch = surface->pitch;
	int Bpp = surface->format->BytesPerPixel;

	printf("surface: %s\n", SDL_GetPixelFormatName(surface->format->format));

	//testing out changing alpha values of a surface
	for (int i = 0;i < surfaceHeight;++i)
	{
		for (int k = 0;k < surfaceWidth;++k)
		{

			//this is probably how SDL_GetRGBA function was made.
			//Uint8 alpha = (pixelInfo >> formatSurface->format->Ashift) & 0xff;
			//Uint8 red = (pixelInfo >> formatSurface->format->Rshift) & 0xff;
			//Uint8 green = (pixelInfo >> formatSurface->format->Gshift) & 0xff;
			//Uint8 blue = (pixelInfo >> formatSurface->format->Bshift) & 0xff;

			//This is probably how SDL_MapRGBA function was made
			//Uint32 newPixel = (alpha << formatSurface->format->Ashift) | (red << formatSurface->format->Rshift) | (green << formatSurface->format->Gshift)
			//	| (blue << formatSurface->format->Bshift);
			//SetPixelInfo(formatSurface, k, i, newPixel);
			SDL_Color color = GetPixelColor(surface, SDL_Point{ k,i });
			if (k >= 100 && k < 150 && i < 66)
				color.a = color.a / 4;
			//color.a = color.a / 4;
			//color.r -= color.r / 4;
			SetPixelColor(surface, SDL_Point{ k,i }, color);
		
		}
	}

	//Note: When creating a texture from surface, the surface's pixel format does not matter
	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);

	int w, h, access;
	Uint32 format;
	SDL_QueryTexture(texture, &format, &access, &w, &h);
	printf("texture format: %s\n", SDL_GetPixelFormatName(format));
	//---------------- Game Loop ------------------//

	//observedDeltaTime is measured in milliseconds
	float observedDeltaTime = core.getTargetDeltaTime();
	float deltaTime = observedDeltaTime / 1000.0f;//converts from milliseconds to seconds
												  //to avoid unnecessary context switches os might do (which I have no control over.. cache the target delta time)
	float targetDeltaTime = core.getTargetDeltaTime();
	Uint64 observedFPS = core.getTargetFPS();
	float currentTime = 0.0f;
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
	bool running = true;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					printf("left mouse click (%d,%d)\n", event.button.x, event.button.y);
				}
			}
		}
		SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
		SDL_RenderClear(render);
		SDL_RenderCopy(render, texture, NULL, NULL);
		SDL_RenderPresent(render);

		endCount = SDL_GetPerformanceCounter();
		observedDeltaTime = (1000.0f * (endCount - startCount)) / performanceFrequency;//gives ms
		observedFPS = performanceFrequency / (endCount - startCount);

		//if the computer can process the update and draw functions faster than 60 fps...
		//cap the frame-rate here to ensure that all computers play roughly around the same fps
		float msDifference = targetDeltaTime - observedDeltaTime;
		if (msDifference > 0)
		{
			SDL_Delay((Uint32)msDifference);
			//Note: must re-record the times after the delay since the times before the delay maybe
			//under 16.666 ms
			endCount = SDL_GetPerformanceCounter();
			observedDeltaTime = (1000.0f * (endCount - startCount)) / performanceFrequency;//gives ms
			observedFPS = performanceFrequency / (endCount - startCount);
		}

		currentTime += observedDeltaTime;
		deltaTime = observedDeltaTime / 1000.0f;
		startCount = endCount;

		std::string title("Beat Em Left");
		title += std::string(" | FPS: ") + std::to_string(observedFPS);
		SDL_SetWindowTitle(core.getWindow(), title.c_str());
	}


	printf("Number of pixels: %d\n", surfaceWidth * surfaceHeight);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

	return 0;
}