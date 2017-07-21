#include "SpriteSheet.h"
#include <stdio.h>
#include <typeinfo.h>

SpriteSheet::SpriteSheet(SDL_Texture* texture,int frameWidth,int frameHeight)
{
	this->texture = NULL;
	type = typeid(SpriteSheet).name();
	currentTime = 0.0f;
	SetTextureAttributes(texture, frameWidth, frameHeight);
}

SpriteSheet::~SpriteSheet()
{
	//delete array texture here
	if (frames != NULL)
	{
		delete[] frames;
		frames = NULL;
	}
}

bool SpriteSheet::SetTextureAttributes(SDL_Texture * srcTexture,int frameWidth,int frameHeight)
{
	if (texture != NULL)
		return false;
	if (srcTexture == NULL)
		return false;

	texture = srcTexture;
	if (SDL_QueryTexture(texture, NULL, NULL, &src_width, &src_height) == -1)
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

	return true;
}

const SDL_Rect* SpriteSheet::GetFrame(int frameIndex)
{
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
