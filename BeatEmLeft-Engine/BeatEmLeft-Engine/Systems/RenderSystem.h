#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

class ComponentManager;
class EntitySystem;

class RenderSystem
{
public:
	RenderSystem();
	~RenderSystem();

	void Update(float deltaTime);

private:
	ComponentManager* spriteManager;
	ComponentManager* transformManager;
	EntitySystem* entitySystem;
};

#endif

