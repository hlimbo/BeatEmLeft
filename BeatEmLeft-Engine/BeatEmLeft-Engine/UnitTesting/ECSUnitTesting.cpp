#include <gtest/gtest.h>
#include "../Systems/ECS.h"
#include "../Systems/ComponentSystem.h"
#include "../Components/GameControllerComponent.h"
#include "SDL2/SDL.h"

TEST(SimpleECS, firstTest)
{
	ECS ecs;

	ecs.CreateComponentSystem(new ComponentSystem("render"));
	ecs.CreateComponentSystem(new ComponentSystem("collision"));
	ecs.CreateComponentSystem(new ComponentSystem("physics"));
	ecs.CreateComponentSystem(new ComponentSystem("boxCollider"));

	EXPECT_EQ(4,ecs.GetComponentTypes().size());

	int playerID = ecs.CreateEntity("player");

	EXPECT_EQ(1, ecs.EntityCount("player"));

	ecs.RemoveEntity("player", playerID);

	EXPECT_ANY_THROW(ecs.EntityCount("player"));

}

TEST(SimpleECS, GameController)
{
	ECS ecs;
	ecs.CreateComponentSystem(new ComponentSystem("GameController"));
	EXPECT_EQ(1, ecs.GetComponentTypes().size());

	int playerID = ecs.CreateEntity("player");
	bool success = ecs.RegisterComponentToEntity(playerID, new GameControllerComponent("GameController"));
	EXPECT_TRUE(success);

	int idRemoved = ecs.RemoveEntity("player", playerID);
	EXPECT_EQ(0, idRemoved);
	EXPECT_EQ(1, ecs.GetComponentTypes().size());

	ecs.DeleteComponentSystem("GameController");
	EXPECT_EQ(0, ecs.GetComponentTypes().size());

}

//interactive unit test... can comment out to run non-interactive tests.
TEST(SimpleECS, GameControllerInitAndUpdate)
{
	ECS ecs;
	ecs.CreateComponentSystem(new ComponentSystem("GameController"));

	int playerID = ecs.CreateEntity("player");
	GameControllerComponent* gc = new GameControllerComponent("GameController");
	ecs.RegisterComponentToEntity(playerID, gc);

	SDL_Init(SDL_INIT_GAMECONTROLLER);
	int startTime = SDL_GetTicks();
	int timeToRunLoop = 15000;//milliseconds
	ecs.Init();
	while (timeToRunLoop > SDL_GetTicks() - startTime)
	{
		ecs.Update(1.0f);
		
	}
	SDL_Quit();
}


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}