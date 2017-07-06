#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

class ECS;
class ComponentManager;
struct SDL_Renderer;

class RenderSystem
{
public:
	RenderSystem();
	RenderSystem(ECS* ecs);
	~RenderSystem();

	void Update(float deltaTime, SDL_Renderer* render);
	void Draw(SDL_Renderer* render);

private:
	//do not need to free these pointers since they get freed in ecs
	ECS* ecs;
	//obtained from ecs
	ComponentManager* transformManager;
	ComponentManager* spriteManager;
};

#endif

