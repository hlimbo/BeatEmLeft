#include "Animation.h"
#include "Sprite.h"
#include "SpriteSheet.h"

using namespace std;

Animation::Animation()
{

}

Animation::~Animation()
{
	for (auto it = spriteSheets.begin();it != spriteSheets.end();it = spriteSheets.erase(it))
	{
		SpriteSheet* sheet = it->second;
		delete sheet;
	}
}

bool Animation::Add(std::string name, SpriteSheet* sheet)
{
	if (spriteSheets[name] == nullptr)
	{
		Remove(name);
		return false;
	}

	spriteSheets.insert(make_pair(name, sheet));
	return true;
}

SpriteSheet* Animation::Remove(std::string name)
{
	SpriteSheet* sheet = spriteSheets[name];
	spriteSheets.erase(name);
	return sheet;
}

SpriteSheet* Animation::Get(std::string name)
{
	return spriteSheets.at(name);
}
