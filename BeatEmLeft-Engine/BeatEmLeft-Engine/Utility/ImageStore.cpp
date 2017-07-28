#include "ImageStore.h"
#include <SDL2/SDL_render.h>
#include <assert.h>

ImageStore::ImageStore(SDL_Renderer* render)
{
	this->render = render;
}

ImageStore::~ImageStore()
{
	int destroyCount = 0;
	for (auto it = images.begin();it != images.end();/*empty*/)
	{
		Image* image = it->second;
		it = images.erase(it);
		destroyCount = TextureLoader::Free(image) ? destroyCount + 1 : destroyCount;
	}

	//debug
	printf("Destroyed Images: %d\n", destroyCount);
}

Image* ImageStore::Load(const std::string fileName, const std::string filePath)
{
	//this is possibly an O(n) check to ensure that the same fileName cannot be added more than once
	if (images.find(fileName) != images.end())
		return nullptr;

	Image* image = TextureLoader::LoadImage(render, filePath.c_str());
	if (image == nullptr)
		return nullptr;

	images.insert(make_pair(fileName, image));
	return image;
}

bool ImageStore::Free(const std::string fileName)
{
	Image* image = images[fileName];
	images.erase(fileName);
	return TextureLoader::Free(image);
}

Image* ImageStore::Get(const std::string fileName)
{
	return images.at(fileName);
}

void ImageStore::SetAlpha(std::string fileName, Uint8 alpha, SDL_BlendMode blendMode)
{
	SDL_Texture* texture = images.at(fileName)->texture;
	SDL_SetTextureBlendMode(texture, blendMode);
	alpha = (alpha > 255) ? 255 : (alpha < 0) ? 0 : alpha;
	SDL_SetTextureAlphaMod(texture, alpha);
}

void ImageStore::SetAlpha(const std::string fileName, Uint8 alpha, SDL_Rect region)
{
	Image* image = images.at(fileName);
	SDL_Surface* surface = image->surface;
	assert(region.x < surface->w);
	assert(region.y < surface->h);
	assert(region.x + region.w <= surface->w);
	assert(region.y + region.h <= surface->h);

	for (int i = region.y;i < region.y + region.h;++i)
	{
		for (int k = region.x;k < region.x + region.w;++k)
		{
			SDL_Color color = GetPixelColor(surface, SDL_Point{ k,i });
			//ignore transparent pixels
			if (color.r == 255 && color.g == 255 && color.b == 255 && color.a == 0)
				continue;
			color.a = alpha;
			SetPixelColor(surface, SDL_Point{ k,i }, color);
		}
	}

	//Update the texture
	SDL_Texture* texture = image->texture;
	TextureLoader::Free(texture);
	images[fileName]->texture = SDL_CreateTextureFromSurface(render, surface);

}

Uint32 ImageStore::GetPixelInfo(void* pixels, int pitch, int Bpp, int x, int y)
{
	//Note: image width = pitch / BytesPerPixel;

	//inside the parenthesis the pixels are 8-bit addressable (Note: can only use pitch and Bpp when the pixels are 8-bit addressable only)
	//outside the parenthesis the pixels become 32-bit addressable (byte addressable)
	return *(Uint32*)((Uint8*)(pixels)+(y * pitch) + (x * Bpp));
}

void ImageStore::SetPixelInfo(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	Uint32* pixels32 = (Uint32*)((Uint8*)surface->pixels + (y * surface->pitch) + (x * surface->format->BytesPerPixel));
	*pixels32 = pixel;

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}

SDL_Color ImageStore::GetPixelColor(SDL_Surface* surface, SDL_Point point)
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

void ImageStore::SetPixelColor(SDL_Surface* surface, SDL_Point point, SDL_Color color)
{
	Uint32 newPixelValue = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
	SetPixelInfo(surface, point.x, point.y, newPixelValue);
}
