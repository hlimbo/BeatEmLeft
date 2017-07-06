#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "../Systems/RenderSystem.h"
#include "../Systems/ComponentManager.h"
#include "../Systems/Component.h"
#include "../Systems/EntitySystem.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Systems/ECS.h"
#include "../Core.h"

using namespace std;

int main(int argc, char* argv[])
{
	//grabbing the write filepaths...
	string mainPath(SDL_GetBasePath());
	mainPath += string("resources\\");
	string playerPath = mainPath + string("blue.png");
	string backgroundPath = mainPath + string("Background.png");

	Core core;
	SDL_Renderer* render = core.getRenderer();
	ECS ecs;

	ecs.CreateComponentManager(new ComponentManager("Transform"));
	ecs.CreateComponentManager(new ComponentManager("Sprite"));

	int entityCount = 10;
	for (int i = 0;i < entityCount;++i)
	{
		int id = ecs.CreateEntity("player");

		auto tc = new TransformComponent("Transform");
		auto sc = new SpriteComponent("Sprite");

		sc->LoadTexture(render, playerPath);
		sc->width = 50;
		sc->height = 50;

		tc->position = Vect2(i * sc->width, 0);
		tc->scale = Vect2(1.0f, 1.0f);

		ecs.RegisterComponentToEntity(id, tc);
		ecs.RegisterComponentToEntity(id, sc);
	}

	RenderSystem renderSystem(&ecs);


	//---------------- Game Loop ------------------//

	float observedDeltaTime = core.getTargetDeltaTime();
	Uint32 observedFPS = core.getTargetFPS();
	float currentTime = 0.0f;
	Uint64 performanceFrequency = SDL_GetPerformanceFrequency();
	Uint64 startCount = SDL_GetPerformanceCounter();
	Uint64 endCount;
	bool running = true;
	while (running)
	{
		//events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
		}

		renderSystem.Update(observedDeltaTime,render);
		renderSystem.Draw(render);

		////cap framerate here
		endCount = SDL_GetPerformanceCounter();
		Uint32 countElapsed = (Uint32)(endCount - startCount);
		observedDeltaTime = 1000.0f * countElapsed / SDL_GetPerformanceFrequency();
		observedFPS = (Uint32)SDL_GetPerformanceFrequency() / countElapsed;
		currentTime += observedDeltaTime;

		//float msDifference = core.getTargetDeltaTime() - observedDeltaTime;

		//if (msDifference != 0)
		//{
		//	if (msDifference > 0)
		//	{
		//		Uint32 msToSleep = (Uint32)(observedDeltaTime + msDifference);
		//		SDL_Delay(msToSleep);
		//	}
		//	else
		//	{
		//		SDL_Delay((Uint32)core.getTargetDeltaTime());
		//	}
		//}

		startCount = endCount;

		//display fps text in title
		if ((int)currentTime % 100 == 0)
		{

			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}


	}


	return 0;
}
