#include <stdio.h>
#include <iostream>
#include <gtest/gtest.h>
#include "../Systems/EntitySystem.h"

TEST(EntityStruct, EntityType)
{
	Entity e1;
	EXPECT_EQ(e1.type, "");
	Entity e2{ "porcelain" };
	EXPECT_EQ(e2.type, "porcelain");
	EXPECT_NE(e1.type, e2.type);
}

TEST(CreateEntity, CreateEntity0)
{
	EntitySystem system;

	Entity e1{ "grass-tile" };
	int id = system.CreateEntity(&e1);
	EXPECT_EQ(e1.type, "grass-tile");

	EXPECT_EQ(id, 0);
}

TEST(EntityType, ContainsEntityType1)
{
	EntitySystem system;
	Entity e1{ "grass-tile" };

	bool hasGrassTile = system.ContainsEntityType(e1.type);
	EXPECT_FALSE(hasGrassTile);
}

TEST(EntityType, AddEntityType)
{
	EntitySystem system;
	system.AddEntityType("grass-tile");
	EXPECT_TRUE(system.ContainsEntityType("grass-tile"));

	system.AddEntityType("water");
	EXPECT_TRUE(system.ContainsEntityType("water"));

	system.AddEntityType("fire");
	EXPECT_TRUE(system.ContainsEntityType("fire"));

	system.AddEntityType("footstool");
	EXPECT_TRUE(system.ContainsEntityType("footstool"));
}

TEST(CreateEntity, CreateEntity1)
{
	EntitySystem system;

	Entity e1{ "cat" };
	int catID = system.CreateEntity(&e1);
	EXPECT_EQ(catID, 0);
	EXPECT_EQ(catID, e1.id);

	Entity e2{ "cat" };
	int newID = system.CreateEntity(&e2);
	EXPECT_EQ(newID, 1);
	EXPECT_EQ(newID, e2.id);
	EXPECT_NE(e1.id, e2.id);

	Entity e3{ "giraffe" };
	int gID = system.CreateEntity(&e3);
	EXPECT_EQ(gID, e3.id);
}

TEST(CreateEntity, DontDuplicateEntityWithSameID)
{
	EntitySystem system;

	Entity e1{ "cat" };
	int status = system.CreateEntity(&e1);
	EXPECT_EQ(e1.id, 0);
	EXPECT_EQ(status, e1.id);

	status = system.CreateEntity(&e1);
	EXPECT_EQ(e1.id, 0);
	EXPECT_EQ(status, -1);
}

TEST(CreateEntity, MultipleEntities)
{
	EntitySystem system;
	Entity e[10];

	for (int i = 0;i < 10; ++i)
	{
		e[i].type = "dog";
		int dogID = system.CreateEntity(&e[i]);
		EXPECT_EQ(dogID, e[i].id);
	}
}

TEST(EntityCount, EntityCountSimple)
{
	EntitySystem system;
	Entity e[10];

	for (int i = 0;i < 10;++i)
	{
		e[i].type = "giraffe";
		system.CreateEntity(&e[i]);
	}

	for (int i = 0;i < 10;++i)
	{
		EXPECT_EQ(e[i].name, e[i].type + to_string(i));
	}

	EXPECT_EQ(system.EntityCount("giraffe"), 10);
}

TEST(AddressCheck, EntityTest)
{
	EntitySystem system;
	Entity e[10];
	for (int i = 0;i < 10;++i)
	{
		e[i].type = "donkey";
		system.CreateEntity(&e[i]);
	}

	vector<Entity*> donkeys = system.GetEntities("donkey");
	for (int i = 0;i < 10;++i)
	{
		printf("donkey: %d | e: %d\n", &donkeys[i], &e[i]);
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}