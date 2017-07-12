#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H
#include <SDL2/SDL_rect.h>
#include <Vect2.h>
#include "ComponentManager.h"

struct Transform;
struct Sprite;
struct ECS;
struct SDL_Renderer;

//a system should take in all the necessary components it needs to process
//an entity is an id mapped to a set of unique components that identify what the entity's functionality is
//each entity can only have 1 of each component type max e.g. an entity cannot be registered to 5 sprite components

class RenderSystem
{
public:
	RenderSystem();
	RenderSystem(ECS* ecs);
	~RenderSystem();

	void Init(int cameraWidth, int cameraHeight);
	//set world coordinates of the camera
	void SetLocation(int x, int y);
	bool SetEntityToFollow(int id);
	void Update(SDL_Renderer* render);
	void Draw(SDL_Renderer* render);

private:
	//do not need to free these pointers since they get freed in ecs
	ECS* ecs;
	//obtained from ecs
	ComponentManager<Transform>* transformManager;
	ComponentManager<Sprite>* spriteManager;
	SDL_Rect camera;//may need to pull out to use in other systems..

	//converts floating point coordinates to integer point coordinates
	SDL_Point getFloatToIntegerCoordinates(Vect2 position);


};

#endif

