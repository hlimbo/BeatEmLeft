#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

#include "../Core.h"
#include "../Systems/EntitySystem.h"
#include "../Systems/ComponentSystem.h"
#include "../Systems/ECS.h"
#include "../Components/KeyboardInputComponent.h"

using namespace std;

int main(int argc, char* argv[])
{
	Core core;

	ECS ecs;
	ecs.CreateComponentSystem(new ComponentSystem("keyboard"));

	int playerID = ecs.CreateEntity("player");
	KeyboardInputComponent* kc = new KeyboardInputComponent("keyboard");
	ecs.RegisterComponentToEntity(playerID, kc);
	ecs.Init();

	vector<string> keyInputs;
	keyInputs.push_back("a");
	keyInputs.push_back("s");
	keyInputs.push_back("d");
	keyInputs.push_back("left");
	keyInputs.push_back("right");
	keyInputs.push_back("up");
	keyInputs.push_back("down");
	keyInputs.push_back("space");

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

		ecs.Update(observedDeltaTime);

		for (int i = 0;i < keyInputs.size(); ++i)
		{
		/*	if (kc->KeyPressed(keyInputs[i]))
			{
				printf("%s is pressed\n", keyInputs[i].c_str());
			}*/
			if (kc->KeyHeld(keyInputs[i]))
			{
				printf("%s is held\n", keyInputs[i].c_str());
			}
			/*if (kc->KeyReleased(keyInputs[i]))
			{
				printf("%s is released\n", keyInputs[i].c_str());
			}*/
		}

		//cap framerate here - TODO: should be wrapped in some function to hide away these details
		endCount = SDL_GetPerformanceCounter();
		Uint32 countElapsed = (Uint32)(endCount - startCount);
		observedDeltaTime = 1000.0f * countElapsed / SDL_GetPerformanceFrequency();
		observedFPS = (Uint32)SDL_GetPerformanceFrequency() / countElapsed;
		currentTime += observedDeltaTime;

		float msDifference = core.getTargetDeltaTime() - observedDeltaTime;

		if (msDifference != 0)
		{
			if (msDifference > 0)
			{
				Uint32 msToSleep = (Uint32)(observedDeltaTime + msDifference);
				SDL_Delay(msToSleep);
			}
			else
			{
				SDL_Delay((Uint32)core.getTargetDeltaTime());
			}
		}

		startCount = endCount;

		if ((int)currentTime % 100 == 0)
		{
			//display fps text in title
			std::string title("Beat Em Left");
			title += std::string(" | FPS: ") + std::to_string(observedFPS);
			SDL_SetWindowTitle(core.getWindow(), title.c_str());
		}
	}

	return 0;
}
