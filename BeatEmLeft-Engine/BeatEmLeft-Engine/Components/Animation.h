#ifndef ANIMATION_H
#define ANIMATION_H
#include <unordered_map>
#include <string>

struct Sprite;
struct SpriteSheet;

//holds a map of animation sheets an entity can use;
//entities can either use sprites("animations with only 1 frame")
//or use sprite sheets ("animations with more than one frame")
struct Animation
{
	Animation();
	~Animation();

	bool Add(std::string name, SpriteSheet* sheet);
	//Note:must delete the sheet you remove from the animation component explicitly
	//e.g. SpriteSheet* idle = animation.Remove("idle");
	//     delete idle;
	SpriteSheet* Remove(std::string name);
	SpriteSheet* Get(std::string name);

private:
	std::unordered_map<std::string, SpriteSheet*> spriteSheets;
};

#endif