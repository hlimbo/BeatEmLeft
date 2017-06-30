#include <stdio.h>
#include <iostream>
#include <gtest/gtest.h>
#include "../Systems/EntitySystem.h"

namespace misc_functions
{
	void InitType(Entity* e, int size, string type)
	{
		for (int i = 0;i < size;++i)
		{
			e[i].type = type;
		}
	}

	void PrintNames(const Entity* e, int size)
	{
		for (int i = 0;i < size;++i)
			cout << e[i].name << endl;
	}
}

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
	const vector<Entity>* entities = system.GetEntities("rice");

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

TEST(RemoveEntity, CantRemoveNonExistentEntityType)
{
	EntitySystem system;
	Entity horses[10];
	misc_functions::InitType(horses, 10, "horses");

	for (int i = 0;i < 10;++i)
		system.CreateEntity(horses[i]);

	int removed = system.RemoveEntity("unicorn", 2);
	EXPECT_EQ(removed, -1);
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

TEST(CreateAndRemove, UniqueIDsPerType)
{
	EntitySystem system;

	Entity e[10];
	misc_functions::InitType(e, 10, "cheese");
	
	for (int i = 0;i < 10;++i)
	{
		system.CreateEntity(e[i]);
	}

	const Entity* ePointer = &(*system.GetEntities("cheese"))[0];

	//2 ways of getting the same thing
	for (int i = 0;i < 10;++i)
	{
		printf("ePointer: %04x | ePtr Long: %04x\n", &ePointer[i], &(*system.GetEntities("cheese"))[i]);
		EXPECT_EQ(&ePointer[i], &(*system.GetEntities("cheese"))[i]);
	}
	cout << endl;
	//my internal unordered_map creates a copy of the Entity into its system
	//This just means another memory address is assigned to the copy of the Entity
	//where the copy of the Entity's address != original Entity's address
	for (int i = 0;i < 10;++i)
	{
		printf("ePointer: %04x | e: %04x\n", &ePointer[i], &e[i]);
		EXPECT_NE(&ePointer[i], &e[i]);
	}
	cout << endl;

	misc_functions::PrintNames(ePointer,10);
	cout << endl;

	system.RemoveEntity("cheese", 2);
	system.RemoveEntity("cheese", 7);
	EXPECT_EQ(system.EntityCount("cheese"), 8);

	misc_functions::PrintNames(ePointer, 10);
	cout << endl;

	Entity e2{ "cheese" };
	Entity e3{ "cheese" };

	system.CreateEntity(e2);
	EXPECT_EQ(9, system.EntityCount("cheese"));

	misc_functions::PrintNames(ePointer, 10);
	cout << endl;

	system.CreateEntity(e3);
	misc_functions::PrintNames(ePointer, 10);
	cout << endl;
	EXPECT_EQ(10, system.EntityCount("cheese"));


	system.CreateEntity(e3);
	misc_functions::PrintNames(ePointer, 10);
	cout << endl;
	EXPECT_NE(11, system.EntityCount("cheese"));

	Entity e4{ "cheese" };
	system.CreateEntity(e4);
	misc_functions::PrintNames(ePointer, 11);
	EXPECT_EQ(11, system.EntityCount("cheese"));

}

TEST(CreateAndRemove, UniqueIDsPerType2)
{
	EntitySystem system;

	Entity e[10];
	misc_functions::InitType(e, 10, "bagel");

	Entity e2[10];
	misc_functions::InitType(e2, 10, "jeep");

	Entity e3[5];
	misc_functions::InitType(e3, 5, "unicorn");

	for (int i = 0;i < 10;++i)
		system.CreateEntity(e[i]);
	for (int i = 0;i < 10;++i)
		system.CreateEntity(e2[i]);
	for (int i = 0;i < 5;++i)
		system.CreateEntity(e3[i]);

	EXPECT_EQ(system.EntityCount("bagel"), 10);
	EXPECT_EQ(system.EntityCount("jeep"), 10);
	EXPECT_EQ(system.EntityCount("unicorn"), 5);
	EXPECT_EQ(system.TypesCount(), 3);

	system.RemoveEntityType("unicorn");
	EXPECT_EQ(system.TypesCount(), 2);

	cout << "Before Removal::" << endl;
	misc_functions::PrintNames(&(*system.GetEntities("jeep"))[0], 10);
	cout << " ---------------- " << endl;
	misc_functions::PrintNames(&(*system.GetEntities("bagel"))[0], 10);
	cout << " ******************************* " << endl << endl;

	//remove every even numbered id in jeep
	for (int i = 0;i < 10; ++i)
	{
		if(i % 2 == 0)
			system.RemoveEntity("jeep", i);
	}

	//remove every odd numbered id in bagel
	for (int i = 0;i < 10; ++i)
	{
		if (i % 2 != 0)
			system.RemoveEntity("bagel", i);
	}

	EXPECT_EQ(system.EntityCount("jeep"), 5);
	EXPECT_EQ(system.EntityCount("bagel"), 5);

	cout << "After Removal::" << endl;
	misc_functions::PrintNames(&(*system.GetEntities("jeep"))[0], 5);
	cout << " ---------------- " << endl;
	misc_functions::PrintNames(&(*system.GetEntities("bagel"))[0], 5);
	cout << endl << endl;

	//add new entities into the system
	Entity newJeeps[15];
	misc_functions::InitType(newJeeps, 15, "jeep");

	Entity newBagels[6];
	misc_functions::InitType(newBagels, 6, "bagel");

	Entity u[3];
	misc_functions::InitType(u, 3, "unicorn");

	for (int i = 0;i < 15;++i)
		system.CreateEntity(newJeeps[i]);
	for (int i = 0;i < 6;++i)
		system.CreateEntity(newBagels[i]);
	for (int i = 0;i < 3;++i)
		system.CreateEntity(u[i]);

	EXPECT_EQ(20, system.EntityCount("jeep"));
	EXPECT_EQ(11, system.EntityCount("bagel"));
	EXPECT_EQ(3, system.EntityCount("unicorn"));

	cout << "After Creation::" << endl;
	for(int i = 0;i < 20;++i)
		cout << system.GetEntity("jeep", i).name << endl;
	cout << "----------------------------" << endl;
	for (int i = 0;i < 11;++i)
		cout << system.GetEntity("bagel", i).name << endl;
	cout << "----------------------------" << endl;
	for (int i = 0;i < 3;++i)
		cout << system.GetEntity("unicorn", i).name << endl;
	cout << "----------------------------" << endl;

}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}