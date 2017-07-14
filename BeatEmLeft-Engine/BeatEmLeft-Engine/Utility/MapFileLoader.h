#ifndef MAP_FILE_LOADER
#define MAP_FILE_LOADER

namespace MapFileLoader
{
	struct TileMap
	{
		TileMap()
		{
			colCount = rowCount = tileWidth = tileHeight = 0;
			contents = nullptr;
		}
		~TileMap()
		{
			if (contents != nullptr)
			{
				for (int r = 0;r < rowCount;++r)
				{
					delete[] contents[r];
					contents[r] = nullptr;
				}
				delete[] contents;
				contents = nullptr;
			}
		}

		int colCount, rowCount;
		int tileWidth, tileHeight;
		int** contents;
	};

	bool Load(const char* filename,TileMap* map);

};
#endif

