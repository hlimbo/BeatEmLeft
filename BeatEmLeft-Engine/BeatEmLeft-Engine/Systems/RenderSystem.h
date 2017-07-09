#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H
#include <SDL2/SDL_rect.h>
#include <Vect2.h>

class ECS;
class ComponentManager;
struct SDL_Renderer;

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
	void Update(float deltaTime, SDL_Renderer* render);
	void Draw(SDL_Renderer* render);

private:
	//do not need to free these pointers since they get freed in ecs
	ECS* ecs;
	//obtained from ecs
	ComponentManager* transformManager;
	ComponentManager* spriteManager;
	SDL_Rect camera;

	//converts floating point coordinates to integer point coordinates
	SDL_Point getFloatToIntegerCoordinates(Vect2 position);


};

#endif

