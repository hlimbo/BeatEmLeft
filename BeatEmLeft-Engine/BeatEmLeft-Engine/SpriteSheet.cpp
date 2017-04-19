#include "SpriteSheet.h"
#include <SDL2/SDL.h>
#include "Texture.h"
#include <stdio.h>



SpriteSheet::SpriteSheet()
{
	texture = new Texture();
}


SpriteSheet::~SpriteSheet()
{
	delete texture;
}

bool SpriteSheet::LoadSpriteSheet(SDL_Renderer * renderer, const char * filepath)
{
	if (texture->LoadTexture(renderer, filepath))
	{
		bounds.x = 0;
		bounds.y = 0;
		int status = SDL_QueryTexture(texture->GetSource(), NULL, NULL, &bounds.w, &bounds.h);
		if (status == 0)
			return true;
	}

	return false;
}

bool SpriteSheet::FreeSpriteSheet()
{
	return texture->FreeTexture();
}


/*
	x = column
	y = row

	if the source texture is 256x256 pixels
	and I wanted to split the picture by 32x32 slices
	I would have 8 slices every row and 8 slices every column
	8 * 8 = 64 slices
	[ 0,0 ] [ 1,0 ] [ 2,0 ] [ 3,0 ] [ 4,0 ] [ 5,0 ] [ 6,0 ] [ 7,0 ]
	[ 0,1 ] [ 1,1 ] [ 2,1 ] [ 3,1 ] [ 4,1 ]
	[ 0,2 ]
	[ 0,3 ]
	[ 0,4 ]
	[ 0,5 ]
	[ 0,6 ]
	[ 0,7 ]

	my task here is to convert 1d index into a 2d representable index

	we know the index, and width and height of image sheet, and slice width and slice height

	number of rows =  sheet.height / slice.height
	number of columns = sheet.width / slice.width

	x = (1d index % number of columns) * slice.width
	y = (1d index /  number of rows) * slice.height

	draw to SDL_Rect src
*/
bool SpriteSheet::DrawFrame(SDL_Renderer * renderer, int frameIndex)
{

	//convert frame index to 2d sprite sheet coordinates
	int numRows = bounds.w / SliceWidth;
	int numCols = bounds.h / SliceHeight;

	framesLength = numRows * numCols;

	if (frameIndex >= framesLength || frameIndex < 0)
	{
		printf("Error: Invalid frame index %d\n", frameIndex);
		return false;
	}

	int sheetX = (frameIndex % numCols) * SliceWidth;
	int sheetY = (frameIndex / numRows) * SliceHeight;

	SDL_Rect src;
	src.h = SliceHeight;
	src.w = SliceWidth;
	src.x = sheetX;
	src.y = sheetY;

	if (SDL_RenderCopy(renderer, texture->GetSource(), &src, &bounds))
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	//SDL_RenderPresent(renderer);
	return true;
}

//x = width in pixels
//y = height in pixels
SDL_Point SpriteSheet::GetSliceDimensions()
{
	return SDL_Point{ SliceWidth, SliceHeight };
}

int SpriteSheet::GetFramesLength()
{
	return framesLength;
}


