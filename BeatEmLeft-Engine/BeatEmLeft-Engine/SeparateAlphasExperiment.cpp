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

//Utilizes Textures,which are stored in vram where the GPU processes the pixel data received
//Modify the alpha values of each pixel on the GPU via Texture Streaming

//Note: its easier to manipulate an SDL_Surface's rgba values than it is manipulating an SDL_Texture's rgba values
//Note: 2 ways to go about this:
//1. manipulate the SDL_Surface's rgba values then create a texture from it.
//2. Use texture streaming to obtain the pixel data from the texture without the need to access SDL_Surface's data.
int main(int argc, char* argv[])
{
	Core core;
	SDL_Renderer* render = core.getRenderer();

	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");
	string objectsPath = mainPath + string("objects.png");
	string redBlockPath = mainPath + string("redblock.png");
	
	//TextureStore store(render);
	//store.Load("objects.png", objectsPath);

	//ECS ecs;

	//int objectsID = ecs.entitySystem.CreateEntity("Objects");
	//auto objectsTransform = new Transform();
	//objectsTransform->position = Vect2(100.0f, 150.0f);
	//auto spriteSheet = new SpriteSheet(store.Get("objects.png"), 50, 66);
	//ecs.transforms.AddComponent(objectsID, objectsTransform);
	//ecs.spriteSheets.AddComponent(objectsID, spriteSheet);

	//Load image
	SDL_Surface* loadedSurface = IMG_Load(redBlockPath.c_str());
	if (loadedSurface == NULL)
	{
		printf("%s\n", IMG_GetError());
		return -1;
	}

	SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL);

	SDL_PixelFormat* pixelFormat = formattedSurface->format;
	const char* pixFormatName = SDL_GetPixelFormatName(pixelFormat->format);
	const Uint8 Bpp = pixelFormat->BytesPerPixel;
	const Uint8 bpp = pixelFormat->BitsPerPixel;
	const Uint32 Rmask = (Uint8)pixelFormat->Rmask;
	const Uint32 Gmask = (Uint8)pixelFormat->Gmask;
	const Uint32 Bmask = (Uint8)pixelFormat->Bmask;
	const Uint32 Amask = pixelFormat->Amask;
	const Uint8 Rloss = pixelFormat->Rloss;
	const Uint8 Gloss = pixelFormat->Gloss;
	const Uint8 Bloss = pixelFormat->Bloss;
	const Uint8 Aloss = pixelFormat->Aloss;
	Uint8 Rshift = pixelFormat->Rshift;
	Uint8 Gshift = pixelFormat->Gshift;
	Uint8 Bshift = pixelFormat->Bshift;
	Uint8 Ashift = pixelFormat->Ashift;

	printf("pixel format name: %s\n", pixFormatName);
	printf("Bytes Per Pixel: %d\n", Bpp);
	printf("Bits Per Pixel: %d\n", bpp);
	printf("Rmask: %04x\n", Rmask);
	printf("Gmask: %04x\n", Gmask);
	printf("Bmask: %04x\n", Bmask);
	printf("Amask: %04x\n", Amask);
	printf("Rloss: %d\n", Rloss);
	printf("Gloss: %d\n", Gloss);
	printf("Bloss: %d\n", Bloss);
	printf("Aloss: %d\n", Aloss);
	printf("Rshift: %d\n", Rshift);
	printf("Gshift: %d\n", Gshift);
	printf("Bshift: %d\n", Bshift);
	printf("Ashift: %d\n", Ashift);


	//by default, textures loaded from surfaces are in pixel format ARGB8888
	//textures loaded from surface also default as SDL_TEXTUREACCESS_STATIC
	//SDL_Texture* someTexture = SDL_CreateTextureFromSurface(render, formattedSurface);
	//Uint32 format;
	//int access;
	//int width;
	//int height;
	//SDL_QueryTexture(someTexture, &format, &access, &width, &height);
	//const char* formatName = SDL_GetPixelFormatName(format);
	//printf("someTexture: %s | access: %d | width: %d | height: %d\n", formatName, access, width, height);

	//set the texture I have here as texture streaming
	SDL_Texture* newTexture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);

	SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

	void* mPixels;
	int mPitch;
	SDL_LockTexture(newTexture,&formattedSurface->clip_rect , &mPixels, &mPitch);

	printf("mPitch: %d\n", mPitch);

	//copy loaded formatted surface pixels ~ copies image from main memory(RAM) to the GPU
	memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);
	//printf("formattedSurface->pitch: %d\n", formattedSurface->pitch);
	//printf("loadedSurface->pitch: %d\n", loadedSurface->pitch);

	//Get pixel data in editable format
	//Uint32* pixels = (Uint32*)mPixels;
	//int pixelCount = (mPitch / 4) * formattedSurface->h;

	SDL_UnlockTexture(newTexture);
	mPixels = NULL;

	//Uint32 format;
	//int access;
	//int width;
	//int height;
	//SDL_QueryTexture(newTexture, &format, &access, &width, &height);
	//const char* formatName = SDL_GetPixelFormatName(format);
	//printf("newTexture: %s | access: %d | width: %d | height: %d\n", formatName, access, width, height);

	//do the lock, unlock texture logic here (decrease each pixel's alpha value within the region specified)

	//SDL_LockTexture
	//lock the first region of the texture (the spear sprite)
	SDL_Rect region;
	region.x = 400;
	region.y = 0;
	region.w = 400;
	region.h = 400;

	Uint8 someAlpha;

	//benefit of this approach is that I can change the SDL_Texture's rgba values without the need to rely on SDL_Surface
	//disadvantage of this approach is its slower to process than processing on SDL_Surface.
	SDL_LockTexture(newTexture, &region, &mPixels, &mPitch);

	//Get pixel data in editable format
	Uint32* pixels = (Uint32*)mPixels;
	int pixelCount = (region.h * (mPitch / Bpp)); //note: mPitch / BytesPerPixel = width of the entire texture
	//int pixelCount = region.h * region.w;
	//int pixelCount = (formattedSurface->h * formattedSurface->w);
	//printf("formattedSurface: %d,%d\n", formattedSurface->h, formattedSurface->w);
	//printf("region.h: %d | mPitch / Bpp: %d\n", region.h, mPitch / Bpp);
	//printf("region.w: %d\n", region.w);
	for (int i = 0;i < pixelCount; ++i)
	{
		//here is sdl's function of grabbing the rgba values
		Uint8 r, g, b, a;
		//to use SDL_GetRGBA: I need to have the SDL_Surface's format!
		SDL_GetRGBA(pixels[i], formattedSurface->format, &r, &g, &b, &a);
	//	printf("%d %d %d %d\n", r, g, b, a);
		Uint32 somePixel = pixels[i];

		//here is the way to grab the rgba values from a 32 bit pixel using bit shifts
		//equivalent to SDL_GetRGBA;
		Uint8 red = (pixels[i] >> 24) & 0xff;
		Uint8 green = (pixels[i] >> 16) & 0xff;
		Uint8 blue = (pixels[i] >> 8) & 0xff;
		Uint8 alpha = (pixels[i]) & 0xff;

		//printf("rgba ( %d, %d, %d, %d)\n", red, green, blue, alpha);

		//modify alpha of a pixel here
		a = (a == 0) ? 0 : a / 2;
		alpha = (alpha == 0) ? 0 : alpha / 2;
		someAlpha = alpha;
		//equivalent to SDL_MapRGBA; (this is without utilizing the pixel format struct from the surface..
		Uint32 color2 = alpha |  (blue << 8) | (green << 16) | (red << 24);
		Uint32 color = SDL_MapRGBA(formattedSurface->format, r, g, b, a);

		pixels[i] = color2;
	}

	//SDL_UnlockTexture
	SDL_UnlockTexture(newTexture);
	mPixels = NULL;

	SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
	SDL_RenderClear(render);

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
		Uint8 deltaAlpha = 8;
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

			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_UP)
				{
					someAlpha += deltaAlpha;
					printf("someAlpha: %d\n", someAlpha);
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					someAlpha -= deltaAlpha;
					printf("someAlpha: %d\n", someAlpha);
				}
			}
		}

		//do the lock, unlock texture logic here (decrease each pixel's alpha value within the region specified)

		//SDL_LockTexture
		//lock the first region of the texture (the spear sprite)
		SDL_Rect region;
		region.x = 400;
		region.y = 0;
		region.w = 400;
		region.h = 400;

		//benefit of this approach is that I can change the SDL_Texture's rgba values without the need to rely on SDL_Surface
		//disadvantage of this approach is its slower to process than processing on SDL_Surface.
		SDL_LockTexture(newTexture, &region, &mPixels, &mPitch);

		//Get pixel data in editable format
		Uint32* pixels = (Uint32*)mPixels;
		int pixelCount = (region.h * (mPitch / Bpp)); //note: mPitch / BytesPerPixel = width of the entire texture
													  //int pixelCount = region.h * region.w;
													  //int pixelCount = (formattedSurface->h * formattedSurface->w);
													  //printf("formattedSurface: %d,%d\n", formattedSurface->h, formattedSurface->w);
													  //printf("region.h: %d | mPitch / Bpp: %d\n", region.h, mPitch / Bpp);
													  //printf("region.w: %d\n", region.w);
		for (int i = 0;i < pixelCount; ++i)
		{
			//here is sdl's function of grabbing the rgba values
			Uint8 r, g, b, a;
			//to use SDL_GetRGBA: I need to have the SDL_Surface's format!
			SDL_GetRGBA(pixels[i], formattedSurface->format, &r, &g, &b, &a);
			//	printf("%d %d %d %d\n", r, g, b, a);
			Uint32 somePixel = pixels[i];

			//here is the way to grab the rgba values from a 32 bit pixel using bit shifts
			//equivalent to SDL_GetRGBA; without the need of its surface!
			Uint8 red = (pixels[i] >> 24) & 0xff;
			Uint8 green = (pixels[i] >> 16) & 0xff;
			Uint8 blue = (pixels[i] >> 8) & 0xff;
			Uint8 alpha = (pixels[i]) & 0xff;

			//printf("rgba ( %d, %d, %d, %d)\n", red, green, blue, alpha);

			//modify alpha of a pixel here
			a = (a == 0) ? 0 : a / 2;
			//alpha = (alpha + deltaAlpha < 0) ? 0 : (alpha + deltaAlpha > 255) ? 255 : alpha + deltaAlpha;
			//alpha = someAlpha < 0 ? 0 : someAlpha > 255 ? 255 : someAlpha;
			
			//equivalent to SDL_MapRGBA; (this is without utilizing the pixel format struct from the surface..
			Uint32 color2 = someAlpha | (blue << 8) | (green << 16) | (red << 24);
			Uint32 color = SDL_MapRGBA(formattedSurface->format, r, g, b, a);

			pixels[i] = color2;
		}

		//SDL_UnlockTexture
		SDL_UnlockTexture(newTexture);
		mPixels = NULL;

		region.x = 0;
		SDL_LockTexture(newTexture, &region, &mPixels, &mPitch);

		//Get pixel data in editable format
		pixels = (Uint32*)mPixels;
		
		pixelCount = (region.h * (mPitch / Bpp)); //note: mPitch / BytesPerPixel = width of the entire texture
													  //int pixelCount = region.h * region.w;
													  //int pixelCount = (formattedSurface->h * formattedSurface->w);
													  //printf("formattedSurface: %d,%d\n", formattedSurface->h, formattedSurface->w);
													  //printf("region.h: %d | mPitch / Bpp: %d\n", region.h, mPitch / Bpp);
													  //printf("region.w: %d\n", region.w);
		for (int i = 0;i < pixelCount; ++i)
		{
			//here is sdl's function of grabbing the rgba values
			Uint8 r, g, b, a;
			//to use SDL_GetRGBA: I need to have the SDL_Surface's format!
			SDL_GetRGBA(pixels[i], formattedSurface->format, &r, &g, &b, &a);
			//	printf("%d %d %d %d\n", r, g, b, a);
			Uint32 somePixel = pixels[i];

			//here is the way to grab the rgba values from a 32 bit pixel using bit shifts
			//equivalent to SDL_GetRGBA; without the need of its surface!
			Uint8 red = (pixels[i] >> 24) & 0xff;
			Uint8 green = (pixels[i] >> 16) & 0xff;
			Uint8 blue = (pixels[i] >> 8) & 0xff;
			Uint8 alpha = (pixels[i]) & 0xff;

			//printf("rgba ( %d, %d, %d, %d)\n", red, green, blue, alpha);

			//modify alpha of a pixel here
			a = (a == 0) ? 0 : a / 2;
			//alpha = (alpha + deltaAlpha < 0) ? 0 : (alpha + deltaAlpha > 255) ? 255 : alpha + deltaAlpha;
			//alpha = someAlpha < 0 ? 0 : someAlpha > 255 ? 255 : someAlpha;

			//equivalent to SDL_MapRGBA; (this is without utilizing the pixel format struct from the surface..
			Uint32 color2 = someAlpha | (blue << 8) | (green << 16) | (red << 24);
			Uint32 color = SDL_MapRGBA(formattedSurface->format, r, g, b, a);

			pixels[i] = color2;
		}

		//SDL_UnlockTexture
		SDL_UnlockTexture(newTexture);
		mPixels = NULL;

		SDL_RenderCopy(render, newTexture, NULL, NULL);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);

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

	SDL_DestroyTexture(newTexture);
	SDL_FreeSurface(formattedSurface);
	SDL_FreeSurface(loadedSurface);



	return 0;
}