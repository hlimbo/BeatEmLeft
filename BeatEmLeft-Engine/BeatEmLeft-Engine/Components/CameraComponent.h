#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H
#include "../Systems/Component.h"
class CameraComponent :
	public Component
{
public:
	CameraComponent(std::string type) : Component(type) {}
	virtual ~CameraComponent() {}

	virtual void Init();
	virtual void Update(float deltaTime);
};

#endif

