#include "SpriteSheet.h"
#include <stdio.h>
#include <typeinfo.h>

SpriteSheet::SpriteSheet()
{
	texture = NULL;
	frames = NULL;
	type = typeid(SpriteSheet).name();
	currentTime = 0.0f;
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
	int rowCount = src_width / this->frameWidth;
	int colCount = src_height / this->frameHeight;

	frameCount = rowCount * colCount;
	frames = new SDL_Rect[frameCount];

	//initialize each frame rect dimensions
	//and each image's position relative to the texture sheet
	for (int i = 0;i < frameCount;++i)
	{
		frames[i].h = this->frameHeight;
		frames[i].w = this->frameWidth;
		frames[i].x = (i % colCount) * frames[i].w;
		frames[i].y = (i / rowCount) * frames[i].h;
	}
	
	currentFrame = frames[0];
	currentFrameIndex = 0;

	//temp code..... might need to change these coordinates regularly if I decide
	//this sprite sheet will be used on a player entity
	bounds.x = 0;
	bounds.y = 0;

	bounds.h = frameWidth * scaleX;
	bounds.w = frameHeight * scaleY;

	return true;
}

const SDL_Rect* SpriteSheet::GetFrame(int frameIndex)
{
	return &frames[frameIndex];
}

void SpriteSheet::PlayAnimation(float deltaTime,float delayTime)
{
	if (currentTime > delayTime)
	{
		currentTime = 0.0f;
		currentFrameIndex = (currentFrameIndex + 1) % frameCount;
		currentFrame = frames[currentFrameIndex];
	}
	else
	{
		currentTime += deltaTime;
	}
}

void SpriteSheet::ResetAnimation()
{
	currentFrameIndex = 0;
}
