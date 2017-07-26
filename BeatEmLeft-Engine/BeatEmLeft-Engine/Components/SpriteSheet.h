#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

struct SpriteSheet
{
	SpriteSheet(SDL_Texture* texture,int frameWidth,int frameHeight);
	~SpriteSheet();

	SDL_Texture* texture;
	//Note:frameWidth and frameHeight is proportional to the actual width and height of the texture
	//when queried
	bool SetTextureAttributes(SDL_Texture* srcTexture, int frameWidth, int frameHeight);

	const SDL_Rect* GetFrame(int frameIndex);
	int GetFrameCount() { return frameCount; }
	const SDL_Rect* GetBounds() { return &bounds; }
	const char* type;

	float scaleX, scaleY;

	//images per second describe the rate at which the frames in the texture change
	//delayTime controls how fast the images on the sprite sheet changed.
	//delayTime is measured in seconds
	SDL_Rect PlayAnimation(float deltaTime,float delayTime);
	void ResetAnimation();

	SDL_Point position;
	SDL_Point offset;

	SDL_Point GetPosition() { return SDL_Point{ position.x - offset.x, position.y - offset.y }; }
	
	int getSrcWidth() { return src_width; }
	int getSrcHeight() { return src_height; }
	int getFrameWidth() { return frameWidth; }
	int getFrameHeight() { return frameHeight; }

private:
	SDL_Rect* frames;
	int frameCount;
	int frameWidth, frameHeight;
	
	float currentTime;//measured in seconds.. used to change frames in PlayAnimation
	int currentFrameIndex;

	//width and height of the entire sprite sheet
	int src_width, src_height;
	//this rect is responsible for displaying the sprite image on screen
	//relative to the game world's coordinates and its width and height
	//can be rescaled to fit properly on the screen
	SDL_Rect bounds;
	//should programmer be given the option to scale the image by pixels?
	//or can the programmer only scale by percentages only? e.g. scaleX and scaleY?
};


#endif
