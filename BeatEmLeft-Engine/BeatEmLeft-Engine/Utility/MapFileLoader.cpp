#include "MapFileLoader.h"
#include <iostream>
#include <fstream>

using namespace std;

bool MapFileLoader::Load(const char* filename, TileMap* map)
{
	ifstream file;
	file.open(filename);
	if (!file.is_open())
	{
		cerr << "File not found: " << filename << endl;
		return false;
	}
	
	file >> map->rowCount;
	file >> map->colCount;
	file >> map->tileHeight;
	file >> map->tileWidth;

	map->contents = new int*[map->rowCount];
	for (int r = 0;r < map->rowCount;++r)
	{
		map->contents[r] = new int[map->colCount];
		for (int c = 0;c < map->colCount;++c)
		{
			file >> map->contents[r][c];
		}
	}

	file.close();
	return true;
}
