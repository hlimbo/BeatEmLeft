/*
	Easy way to include ALL HEADER FILES of the project without the need to include each header file separately
	when creating new experimental .cpp files
*/


#ifndef MASTER_HEADER_H
#define MASTER_HEADER_H

//CORE
#include "Core.h"
#include "GameConstants.h"
#include "GameLoop.h"

// COMPONENTS
#include "Components/Animation.h"
#include "Components/BoxCollider.h"
#include "Components/CircleCollider.h"
#include "Components/Kinematic.h"
#include "Components/SlopeCollider.h"
#include "Components/Sprite.h"
#include "Components/SpriteSheet.h"
#include "Components/Transform.h"

//INPUT
#include "Input/GameController.h"
#include "Input/KeyboardController.h"

//SYSTEMS
#include "Systems/AnimationStateMachine.h"
#include "Systems/ComponentManager.h"
#include "Systems/ECS.h"
#include "Systems/EntitySystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/RenderSystem.h"

////UTILITY
//#include "Utility/CollisionQuery.h"//linker issues caused here
#include "Utility/ImageMod.h"
#include "Utility/ImageStore.h"
#include "Utility/StaticTextStore.h"
#include "Utility/GUI.h"

#endif
