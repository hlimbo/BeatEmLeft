#include "ImageMod.h"
#include <assert.h>
#include "../Utility/TextureLoader.h"

void ImageMod::SetAlpha(Image* image, Uint8 alpha, SDL_BlendMode blendMode)
{
	SDL_Texture* texture = image->texture;
	SDL_SetTextureBlendMode(texture, blendMode);
	alpha = (alpha > 255) ? 255 : (alpha < 0) ? 0 : alpha;
	SDL_SetTextureAlphaMod(texture, alpha);
}

void ImageMod::SetAlpha(Image* image, Uint8 alpha, SDL_Rect region, SDL_Renderer* render)
{
	SDL_Surface* surface = image->surface;
	assert(region.x < surface->w);
	assert(region.y < surface->h);
	assert(region.x + region.w <= surface->w);
	assert(region.y + region.h <= surface->h);


	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
	for (int i = region.y;i < region.y + region.h;++i)
	{
		for (int k = region.x;k < region.x + region.w;++k)
		{
			SDL_Color color = GetPixelColor(surface, SDL_Point{ k,i });
			//ignore transparent pixels
	/*		if (color.r == 255 && color.g == 255 && color.b == 255 && color.a == 0)
				continue;*/
			color.a = alpha;
			SetPixelColor(surface, SDL_Point{ k,i }, color);
		}
	}

	//Update the texture
	SDL_Texture* texture = image->texture;
	TextureLoader::Free(texture);
	image->texture = SDL_CreateTextureFromSurface(render, surface);
}

Uint32 ImageMod::GetPixelInfo(void* pixels, int pitch, int Bpp, int x, int y)
{
	//Note: image width = pitch / BytesPerPixel;

	//inside the parenthesis the pixels are 8-bit addressable (Note: can only use pitch and Bpp when the pixels are 8-bit addressable only)
	//outside the parenthesis the pixels become 32-bit addressable (byte addressable)
	return *(Uint32*)((Uint8*)(pixels)+(y * pitch) + (x * Bpp));
}

void ImageMod::SetPixelInfo(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	Uint32* pixels32 = (Uint32*)((Uint8*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel));
	*pixels32 = pixel;

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

SDL_Color ImageMod::GetPixelColor(SDL_Surface* surface, SDL_Point point)
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

void ImageMod::SetPixelColor(SDL_Surface* surface, SDL_Point point, SDL_Color color)
{
	Uint32 newPixelValue = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
	SetPixelInfo(surface, point.x, point.y, newPixelValue);
}
