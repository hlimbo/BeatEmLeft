#include <gtest/gtest.h>
#include "../Systems/ComponentSystem.h"
#include "../Systems/EntitySystem.h"
#include "../Systems/Component.h"

//These are bad unit tests -- TODO: need to rewrite them
TEST(miniTest, miniTest)
{
	unordered_map<int, string*> stuffs;
	stuffs.emplace(95, new string("hello"));

	cout << stuffs.empty() << endl;
	cout << *stuffs[95] << endl;
	string* ptr = stuffs[95];
	
	EXPECT_EQ(ptr, stuffs[95]);
	cout << ptr << " == " << stuffs[95] << endl;

	stuffs.erase(95);
	cout << stuffs.empty() << endl;

	cout << *ptr << endl;
	delete ptr;
	cout << ptr << endl;
	ptr = nullptr;
	cout << ptr << endl;
}

TEST(SimpleTests, AddComponent)
{

	ComponentSystem renderSystem;//this is implicitly a renderComponentSystem.
	EntitySystem eSystem;

	Component component("render");
	eSystem.CreateEntity("player");

	int playerID = eSystem.GetEntities("player")->at(0).id;
	renderSystem.AddComponent(playerID, &component);

	//this is obtaining a render component ~ might not be clear when reading this tho
	Component* renderComponent = renderSystem.GetComponent(playerID);
	
	EXPECT_EQ(renderComponent, &component);
	EXPECT_EQ(1, renderSystem.GetComponentCount());

}

TEST(SimpleTests, GetComponent)
{
	ComponentSystem cSystem;
	EntitySystem eSystem;

	Component component("component");
	eSystem.CreateEntity("entity");

	int entityID = eSystem.GetEntities("entity")->at(0).id;
	cSystem.AddComponent(entityID, &component);

	Component* compPtr = cSystem.GetComponent(entityID);
	EXPECT_EQ(&component, compPtr);
	EXPECT_EQ(component.GetType(), compPtr->GetType());

}

TEST(SimpleTests, RemoveComponent)
{
	ComponentSystem renderSystem;//implicitly declared as a componentSystem that only manages renderComponents
	EntitySystem eSystem;

	Component component("render");
	eSystem.CreateEntity("tile");

	int tileID = eSystem.GetEntities("tile")->at(0).id;
	renderSystem.AddComponent(tileID, &component);
	EXPECT_EQ(1, renderSystem.GetComponentCount());

	Component* renderComponent = renderSystem.RemoveComponent(tileID);
	EXPECT_EQ(renderComponent, &component);
	EXPECT_EQ(0, renderSystem.GetComponentCount());
}

TEST(EntityComponents, CheckEntityForComponents)
{
	ComponentSystem moveSystem;
	ComponentSystem renderSystem;
	ComponentSystem physicsSystem;
	ComponentSystem collisionSystem;

	Component moveComponent("move");
	Component renderComponent("render");
	Component physicsComponent("physics");
	Component collisionComponent("collision");

	EntitySystem entitySystem;

	//here are 2 ways of getting the playerID
	int playerID = entitySystem.CreateEntity("player");
	playerID = entitySystem.GetEntities("player")->at(0).id;

	moveSystem.AddComponent(playerID, &moveComponent);
	renderSystem.AddComponent(playerID, &renderComponent);
	physicsSystem.AddComponent(playerID, &physicsComponent);
	collisionSystem.AddComponent(playerID, &collisionComponent);

	EXPECT_EQ(moveSystem.GetComponentCount(), 1);
	EXPECT_EQ(renderSystem.GetComponentCount(), 1);
	EXPECT_EQ(physicsSystem.GetComponentCount(), 1);
	EXPECT_EQ(collisionSystem.GetComponentCount(), 1);

	//whenever an entity is removed from the system.. we must unregister its
	//id from all the component systems
	entitySystem.RemoveEntity("player", playerID);

	//Question: After an entity has been removed from the entitySystem...
	//how do I know which systems to remove component from?

	moveSystem.RemoveComponent(playerID);
	renderSystem.RemoveComponent(playerID);
	physicsSystem.RemoveComponent(playerID);
	collisionSystem.RemoveComponent(playerID);

	EXPECT_EQ(moveSystem.GetComponentCount(), 0);
	EXPECT_EQ(renderSystem.GetComponentCount(), 0);
	EXPECT_EQ(physicsSystem.GetComponentCount(), 0);
	EXPECT_EQ(collisionSystem.GetComponentCount(), 0);

	//TODO: Create a function that checks the number of components
	//an entity is registered to
	
	//e.g. 
	//const Entity* player = entitySystem.GetEntities("player")->at(0);
	//cout << player->GetComponentsCount() << endl;

	//TODO: Create a function that returns a list of Component ptrs
	//that entity is registered to ~ might not do...
	//might not do because entity class will need to do some book keeping
	//which includes removing component ptrs from the list that were removed from other component systems already. 

	//e.g. vector<Component*> components;
	//const Entity* player = entitySystem.GetEntities("player")->at(0);
	//	for(Component* component : player->GetComponents())
	//
	
}

TEST(UnorderdMapPointers, Pointers)
{
	unordered_map<int, Component*> mymap;
	EXPECT_EQ(mymap[0], nullptr);
}

TEST(NullPointers, ComponentSystem)
{
	ComponentSystem cSystem;

	Component* component = cSystem.GetComponent(999);
	EXPECT_EQ(nullptr, component);
	EXPECT_TRUE(cSystem.IsEmpty());

	Component bob("bob");
	cSystem.AddComponent(45, &bob);

	EXPECT_EQ(1, cSystem.GetComponentCount());
	component = cSystem.GetComponent(45);
	EXPECT_TRUE(component != nullptr);

	cSystem.RemoveComponent(45);
	EXPECT_TRUE(cSystem.IsEmpty());
	component = cSystem.GetComponent(45);
	EXPECT_FALSE(component != nullptr);

	//removing a component using an invalid id
	component = cSystem.RemoveComponent(2);
	EXPECT_EQ(nullptr, component);
	EXPECT_EQ(0, cSystem.GetComponentCount());
}

TEST(Components, AddDuplicateComponents)
{
	ComponentSystem cSystem;

	//this following test maps different ids to the same exact copy
	//of the component duplicate 10 times.
	Component duplicate("dupe");

	for (int i = 0;i < 10;++i)
		cSystem.AddComponent(i, &duplicate);

	EXPECT_EQ(10, cSystem.GetComponentCount());
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}