#include "SpriteSheet.h"
#include "../Utility/TextureLoader.h"
#include "../Utility/ImageMod.h"
#include <stdio.h>
#include <typeinfo.h>
#include <assert.h>

SpriteSheet::SpriteSheet(SDL_Renderer* render,SDL_Texture* texture,int frameWidth,int frameHeight)
{
	this->render = render;
	this->texture = NULL;
	type = typeid(SpriteSheet).name();
	currentTime = 0.0f;
	SetTextureAttributes(texture, frameWidth, frameHeight);
	scaleX = 1.0f;
	scaleY = 1.0f;
}

SpriteSheet::SpriteSheet(SDL_Renderer* render,Image* image, int frameWidth, int frameHeight)
{
	this->render = render;
	this->texture = NULL;
	this->image = image;
	type = typeid(SpriteSheet).name();
	currentTime = 0.0f;
	SetTextureAttributes(image->texture, frameWidth, frameHeight);
	this->texture = this->image->texture;
	scaleX = 1.0f;
	scaleY = 1.0f;
}

SpriteSheet::~SpriteSheet()
{
	//delete array texture here
	if (frames != NULL)
	{
		delete[] frames;
		frames = NULL;
	}

	if (alphas != NULL)
	{
		delete[] alphas;
		alphas = NULL;
	}
}

bool SpriteSheet::SetTextureAttributes(SDL_Texture* srcTexture,int frameWidth,int frameHeight)
{
	if (texture != NULL)
		return false;
	if (srcTexture == NULL)
		return false;

	texture = srcTexture;
	int accessMode;
	if (SDL_QueryTexture(texture, NULL, &accessMode, &src_width, &src_height) == -1)
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		return false;
	}

	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	int colCount = src_width / this->frameWidth;
	int rowCount = src_height / this->frameHeight;

	frameCount = rowCount * colCount;
	frames = new SDL_Rect[frameCount];

	int r = 0;
	int c = 0;

	//initialize each frame rect dimensions
	//and each image's position relative to the texture sheet
	for (int i = 0;i < frameCount;++i)
	{
		frames[i].h = this->frameHeight;
		frames[i].w = this->frameWidth;

		//i = colCount * rowIndex + colIndex;
		//colIndex = i - (colCount * rowIndex);
		//rowIndex = (i - colIndex) / colCount;

		c = i - (colCount * r);
		if (c >= colCount)
		{
			c = 0;
			++r;
		}

		if (rowCount == 1)
			r = 0;

		frames[i].x = c * frames[i].w;
		frames[i].y = r * frames[i].h;
	}
	
	currentFrameIndex = 0;

	//temp code..... might need to change these coordinates regularly if I decide
	//this sprite sheet will be used on a player entity
	bounds.x = 0;
	bounds.y = 0;

	bounds.h = (int)(frameWidth * scaleX);
	bounds.w = (int)(frameHeight * scaleY);

	//initialize each frame's alpha value by retrieving each frame's alpha value
	alphas = new Uint8[frameCount];

	return true;
}

//Note:Sometimes setting the alpha of specific portions of the texture cause errors when 
//TextureStore tries to delete the textures that have modified alpha values.
void SpriteSheet::SetAlpha(int frameIndex, Uint8 newAlpha)
{
	assert(frameIndex >= 0 && frameIndex < frameCount);
	assert(texture != NULL);
	assert(image->texture != NULL);

	ImageMod::SetAlpha(image, newAlpha, frames[frameIndex], render);
	assert(texture != NULL);
	texture = image->texture;
	alphas[frameIndex] = newAlpha;
}

//I need this in spritesheet because I might want to change individual portions of the sprite sheets alpha values
void SpriteSheet::SetAlpha(Uint8 newAlpha,SDL_BlendMode blendMode)
{
	assert(texture != NULL);
	assert(image->texture != NULL);
	ImageMod::SetAlpha(image, newAlpha, blendMode);

	for(int i = 0;i < frameCount;++i)
		alphas[i] = newAlpha;
}

const SDL_Rect* SpriteSheet::GetFrame(int frameIndex)
{
	assert(frameIndex >= 0 && frameIndex < frameCount);
	return &frames[frameIndex];
}

SDL_Rect SpriteSheet::PlayAnimation(float deltaTime,float delayTime)
{
	if (currentTime > delayTime)
	{
		currentTime = 0.0f;
		currentFrameIndex = (currentFrameIndex + 1) % frameCount;
	}
	else
	{
		currentTime += deltaTime;
	}

	return frames[currentFrameIndex];
}

void SpriteSheet::ResetAnimation()
{
	currentFrameIndex = 0;
}
