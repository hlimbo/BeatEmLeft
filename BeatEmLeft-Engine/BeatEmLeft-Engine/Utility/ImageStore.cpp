#include "ImageStore.h"
#include <SDL2/SDL_render.h>
#include <assert.h>
#include "../Utility/ImageMod.h"

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
	//return the image that is already loaded into the store
	if (images.find(fileName) != images.end())
		return images[fileName];

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
	assert(images[fileName] != nullptr);
	return images.at(fileName);
}

void ImageStore::SetAlpha(std::string fileName, Uint8 alpha, SDL_BlendMode blendMode)
{
	Image* image = images.at(fileName);
	ImageMod::SetAlpha(image, alpha,blendMode);
}


void ImageStore::SetAlpha(const std::string fileName, Uint8 alpha, SDL_Rect region)
{
	Image* image = images.at(fileName);
	ImageMod::SetAlpha(image, alpha,region,render);
}
