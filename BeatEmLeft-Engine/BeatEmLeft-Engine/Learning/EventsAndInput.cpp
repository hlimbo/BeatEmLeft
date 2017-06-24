#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

#define WIDTH 640
#define HEIGHT 480

int main(int argv, char** argc)
{
	SDL_Init(SDL_INIT_EVENTS);

	SDL_Window* window = SDL_CreateWindow("Events and Input", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					cout << "Up key pressed at " << event.key.timestamp << endl;
					break;
				case SDLK_DOWN:
					cout << "Down key pressed at " << event.key.timestamp << endl;
					break;
				case SDLK_LEFT:
					cout << "Left key pressed at " << event.key.timestamp << endl;
					break;
				case SDLK_RIGHT:
					cout << "Right key pressed at " << event.key.timestamp << endl;
					break;
				default:
					cout << "Some key pressed at " << event.key.timestamp << endl;
					break;
				}
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				//left mouse button = 1
				//middle mouse button = 2
				//right mouse button = 3
				cout << "mouse button: " << event.button.button << " at timestamp: " << event.button.timestamp << endl;
				cout << endl;
			}
			else if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}

	SDL_Quit();
	return 0;
}