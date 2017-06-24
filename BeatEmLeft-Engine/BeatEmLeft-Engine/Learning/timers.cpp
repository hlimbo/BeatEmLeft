#include <SDL2/SDL.h>
#include <iostream>
using namespace std;


//simple gameloop ~ TAKES UP a good chunk of CPU
int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	unsigned int lastTime = 0, currentTime;

	unsigned int updateFrequency = 500;
	while (true)
	{
		currentTime = SDL_GetTicks();
		if (currentTime >= lastTime + updateFrequency)
		{
			//cout << (currentTime - lastTime) << " ms have passed " << endl;
			lastTime = currentTime;
		}
	}



	SDL_Quit();
	return 0;
}