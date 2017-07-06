#include <gtest/gtest.h>
#include "../Systems/ECS.h"
#include "../Systems/ComponentManager.h"
#include "../Components/GameControllerComponent.h"
#include "../Components/KeyboardInputComponent.h"
#include "SDL2/SDL.h"

TEST(SimpleECS, firstTest)
{
	ECS ecs;

	ecs.CreateComponentManager(new ComponentManager("render"));
	ecs.CreateComponentManager(new ComponentManager("collision"));
	ecs.CreateComponentManager(new ComponentManager("physics"));
	ecs.CreateComponentManager(new ComponentManager("boxCollider"));

	EXPECT_EQ(4,ecs.GetComponentTypes().size());

	int playerID = ecs.CreateEntity("player");

	EXPECT_EQ(1, ecs.EntityCount("player"));

	ecs.RemoveEntity("player", playerID);

	EXPECT_ANY_THROW(ecs.EntityCount("player"));

}

TEST(SimpleECS, GameController)
{
	ECS ecs;
	ecs.CreateComponentManager(new ComponentManager("GameController"));
	EXPECT_EQ(1, ecs.GetComponentTypes().size());

	int playerID = ecs.CreateEntity("player");
	bool success = ecs.RegisterComponentToEntity(playerID, new GameControllerComponent("GameController"));
	EXPECT_TRUE(success);

	int idRemoved = ecs.RemoveEntity("player", playerID);
	EXPECT_EQ(0, idRemoved);
	EXPECT_EQ(1, ecs.GetComponentTypes().size());

	ecs.DeleteComponentManager("GameController");
	EXPECT_EQ(0, ecs.GetComponentTypes().size());

}

TEST(SimpleECS, CheckComponentTypes)
{
	Component* keyboard = new KeyboardInputComponent("keyboard");
	Component* gamecontroller = new GameControllerComponent("gamecontroller");

	EXPECT_EQ("keyboard", keyboard->GetType());
	EXPECT_EQ("gamecontroller", gamecontroller->GetType());

	KeyboardInputComponent* kc = static_cast<KeyboardInputComponent*>(keyboard);
	GameControllerComponent* gc = static_cast<GameControllerComponent*>(gamecontroller);

	EXPECT_EQ(kc, keyboard);
	EXPECT_EQ(gc, gamecontroller);

	delete keyboard;
	delete gamecontroller;
}

//interactive unit test... can comment out to run non-interactive tests.
//TEST(InteractiveTest, GameControllerInitAndUpdate)
//{
//	ECS ecs;
//	ecs.CreateComponentSystem(new ComponentManager("GameController"));
//
//	int playerID = ecs.CreateEntity("player");
//	GameControllerComponent* gc = new GameControllerComponent("GameController");
//	ecs.RegisterComponentToEntity(playerID, gc);
//
//	SDL_Init(SDL_INIT_GAMECONTROLLER);
//	int startTime = SDL_GetTicks();
//	int timeToRunLoop = 15000;//milliseconds
//	ecs.Init();
//	while (timeToRunLoop > SDL_GetTicks() - startTime)
//	{
//		ecs.Update(1.0f);	
//	}
//	SDL_Quit();
//}


//can't test keyboard inputs here :(
//TEST(InteractiveTest, KeyboardInputComponentTest)
//{
//	ECS ecs;
//	ecs.CreateComponentSystem(new ComponentManager("Keyboard"));
//
//	int playerID = ecs.CreateEntity("player");
//	KeyboardInputComponent* kc = new KeyboardInputComponent("Keyboard");
//	ecs.RegisterComponentToEntity(playerID, kc);
//
//	SDL_Init(SDL_INIT_EVERYTHING);
//	int startTime = SDL_GetTicks();
//	int timeToRunLoop = 15000;//ms
//	ecs.Init();
//	while (timeToRunLoop > SDL_GetTicks() - startTime)
//	{
//		ecs.Update(1.0f);
//	}
//
//	//while (timeToRunLoop > (int)SDL_GetTicks() - startTime)
//	//{
//	//	SDL_Event event;
//	//	while (SDL_PollEvent(&event))
//	//	{
//	//		switch (event.type)
//	//		{
//	//		case SDL_KEYDOWN:
//	//			puts("A key was pressed");
//	//			break;
//	//		case SDL_KEYUP:
//	//			puts("A key was released");
//	//			break;
//	//		}
//	//	}
//	//}
//
//	SDL_Quit();
//}


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}