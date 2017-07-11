#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <SDL2/SDL_render.h>
#include "../Systems/Component.h"

struct SDL_Texture;
struct SDL_Renderer;

class SpriteComponent :
	public Component
{
public:
	SpriteComponent(std::string type);
	virtual ~SpriteComponent();

	//may need to rewrite component base class to not include
	//inheritable functions via virtual keyword.
	virtual void Init();
	virtual void Update(float deltaTime);

	void Draw(SDL_Renderer* render);

	//helper function
	SDL_Texture* texture;
	bool SetTextureAttributes(SDL_Texture* srcTexture);

	//modify this width and height to see changes in size of image on screen
	int width, height;
	//pixel coordinates of image
	int x, y;
	//flipping the image
	SDL_RendererFlip flip;
private:
	//source width and height of image after loaded into the component
	int src_w, src_h;

};

#endif

