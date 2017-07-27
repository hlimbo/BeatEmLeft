#include "SpriteSheet.h"
#include <stdio.h>
#include <typeinfo.h>
#include <assert.h>

SpriteSheet::SpriteSheet(SDL_Texture* texture,int frameWidth,int frameHeight)
{
	this->texture = NULL;
	type = typeid(SpriteSheet).name();
	currentTime = 0.0f;
	SetTextureAttributes(texture, frameWidth, frameHeight);
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

bool SpriteSheet::SetTextureAttributes(SDL_Texture * srcTexture,int frameWidth,int frameHeight)
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

	if (accessMode != SDL_TEXTUREACCESS_STREAMING)
	{
		fprintf(stderr, "Error: target texture needs to have access value of: SDL_TEXTUREACCESS_STREAMING\n");
		return false;
	}

	this->frameWidth = frameWidth;
	this->frameHeight = frameHeight;
	int colCount = src_width / this->frameWidth;
	int rowCount = src_height / this->frameHeight;

	frameCount = rowCount * colCount;
	frames = new SDL_Rect[frameCount];

	//initialize each frame rect dimensions
	//and each image's position relative to the texture sheet
	for (int i = 0;i < frameCount;++i)
	{
		frames[i].h = this->frameHeight;
		frames[i].w = this->frameWidth;

		//edge case for sprite sheets that can be 1x5's or 5x1's
		int r = rowCount == 1 ? 0 : i;
		int c = colCount == 1 ? 0 : i;

		frames[i].x = (c % colCount) * frames[i].w;
		frames[i].y = (r / rowCount) * frames[i].h;
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
	for (int i = 0;i < frameCount;++i)
	{
		void* mPixels;
		int mPitch;
		SDL_LockTexture(texture, &frames[i], &mPixels, &mPitch);
		Uint32* pixels = (Uint32*)mPixels;
		alphas[i] = pixels[0] & 0xff;
		SDL_UnlockTexture(texture);
		mPixels = NULL;
	}

	return true;
}

void SpriteSheet::SetAlpha(int frameIndex, Uint8 newAlpha)
{
	assert(frameIndex >= 0 && frameIndex < frameCount);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	void* mPixels;
	int mPitch;
	SDL_LockTexture(texture, &frames[frameIndex], &mPixels, &mPitch);

	Uint32* pixels = (Uint32*)mPixels;
	int pixelCount = frames[frameIndex].h * (mPitch / 4);
	for (int i = 0;i < pixelCount;++i)
	{
		Uint8 red = (pixels[i] >> 24) & 0xff;
		Uint8 green = (pixels[i] >> 16) & 0xff;
		Uint8 blue = (pixels[i] >> 8) & 0xff;
		Uint8 alpha = pixels[i] & 0xff;

		if (newAlpha > 255) newAlpha = 255;
		if (newAlpha < 0) newAlpha = 0;
		Uint32 color = newAlpha | (blue << 8) | (green << 16) | (red << 24);
		pixels[i] = color;
	}

	SDL_UnlockTexture(texture);
	mPixels = NULL;
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
