#ifndef ECS_H
#define ECS_H

class EntitySystem;
class ComponentSystem;

//Entity-Component System (ECS)

class ECS
{
public:
	ECS();
	~ECS();
private:
	EntitySystem* entitySystem;
	ComponentSystem* componentSystem;
};

#endif

