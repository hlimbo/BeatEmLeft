#include <gtest/gtest.h>
#include "../Systems/ComponentSystem.h"

TEST(dummy, dummy)
{
	EXPECT_TRUE(true);
}

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

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}