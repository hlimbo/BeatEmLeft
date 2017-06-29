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
	int id = system.CreateEntity(e1);
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
	int catID = system.CreateEntity(e1);
	EXPECT_EQ(catID, 0);
	EXPECT_EQ(catID, e1.id);

	Entity e2{ "cat" };
	int newID = system.CreateEntity(e2);
	EXPECT_EQ(newID, 1);
	EXPECT_EQ(newID, e2.id);
	EXPECT_NE(e1.id, e2.id);

	Entity e3{ "giraffe" };
	int gID = system.CreateEntity(e3);
	EXPECT_EQ(gID, e3.id);
}

TEST(CreateEntity, DontDuplicateEntityWithSameID)
{
	EntitySystem system;

	Entity e1{ "cat" };
	int status = system.CreateEntity(e1);
	EXPECT_EQ(e1.id, 0);
	EXPECT_EQ(status, e1.id);

	status = system.CreateEntity(e1);
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
		int dogID = system.CreateEntity(e[i]);
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
		system.CreateEntity(e[i]);
	}

	for (int i = 0;i < 10;++i)
	{
		EXPECT_EQ(e[i].name, e[i].type + to_string(i));
	}

	EXPECT_EQ(system.EntityCount("giraffe"), 10);
}


TEST(RemoveEntity, RemoveEntity1)
{
	EntitySystem system;
	Entity e[10];
	for (int i = 0;i < 10;++i)
	{
		e[i].type = "rice";
		system.CreateEntity(e[i]);
	}
	
	EXPECT_EQ(system.EntityCount("rice"), 10);
	vector<Entity>* entities = system.GetEntities("rice");

	for (int i = 0;i < system.EntityCount("rice"); ++i)
	{
		//cout << system.entitiesByType["rice"][i].name << endl;
		//cout << ((*entities)[i]).name<< endl;// alternate way of doing the same thing below
		cout << entities->at(i).name << endl;
	}

	int idToRemove = system.RemoveEntity("rice", 4);
	EXPECT_EQ(idToRemove, 4);
	cout << endl << endl;
	EXPECT_EQ(system.EntityCount("rice"), 9);
	for (int i = 0;i < system.EntityCount("rice"); ++i)
	{
		//cout << system.entitiesByType["rice"][i].name << endl;
		cout << entities->at(i).name << endl;
	}

	cout << endl << endl;

	idToRemove = system.RemoveEntity("rice", 4);
	EXPECT_EQ(idToRemove, -1);
	EXPECT_EQ(system.EntityCount("rice"), 9);
	for (int i = 0;i < system.EntityCount("rice"); ++i)
	{
		//cout << system.entitiesByType["rice"][i].name << endl;
		cout << entities->at(i).name << endl;
	}

	cout << endl << endl;

	idToRemove = system.RemoveEntity("rice", 0);
	EXPECT_EQ(idToRemove, 0);
	EXPECT_EQ(system.EntityCount("rice"), 8);
	for (int i = 0;i < system.EntityCount("rice"); ++i)
	{
		//cout << system.entitiesByType["rice"][i].name << endl;
		cout << entities->at(i).name << endl;
	}

}

TEST(RemoveEntityType, RemoveEntityType1)
{
	EntitySystem system;
	system.AddEntityType("cheese");
	system.AddEntityType("bread");
	system.AddEntityType("donut");

	Entity e[20];
	for (int i = 0;i < 20;++i)
	{
		e[i].type = "bread";
		system.CreateEntity(e[i]);
	}

	EXPECT_TRUE(system.RemoveEntityType("bread"));
	EXPECT_FALSE(system.RemoveEntityType("bread"));
	EXPECT_FALSE(system.RemoveEntityType("bchk"));
	EXPECT_EQ(system.TypesCount(), 2);

	EXPECT_TRUE(system.RemoveEntityType("donut"));
	EXPECT_EQ(system.TypesCount(), 1);

	EXPECT_TRUE(system.RemoveEntityType("cheese"));
	EXPECT_EQ(system.TypesCount(), 0);

}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}