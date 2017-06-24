#include <SDL2/SDL.h>
#include <iostream>
using namespace std;

#define WIDTH 640
#define HEIGHT 480

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Basic Graphics", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!window || !renderer)
		cout << SDL_GetError() << endl;
	
	int width, height;
	SDL_GetRendererOutputSize(renderer, &width, &height);

	cout << "Render output size: " << width << ", " << height << endl;


	SDL_Point points[4] = {
		{ 320, 200 },
		{ 300, 240 },
		{ 340, 240 },
		{ 320, 240 }
	};

	SDL_Rect rect;
	rect.h = 50;
	rect.w = 50;
	rect.x = 400;
	rect.y = 200;

	bool quit = false;
	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			//sets the background color to black
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);
			
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawLine(renderer, 0, 0, 300, 240);
			//SDL_RenderDrawLines(renderer, points, 4);
			//SDL_RenderDrawPoint(renderer, WIDTH / 2, HEIGHT / 2);
			SDL_SetRenderDrawColor(renderer, 0,255,0, SDL_ALPHA_OPAQUE);
			SDL_RenderFillRect(renderer, &rect);
			SDL_RenderDrawRect(renderer, &rect);
			SDL_RenderPresent(renderer);
		}
	}

	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}

//int main(int argc, char* argv[])
//{
//	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
//		SDL_Window* window = NULL;
//		SDL_Renderer* renderer = NULL;
//
//		if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) == 0) {
//			SDL_bool done = SDL_FALSE;
//
//			while (!done) {
//				SDL_Event event;
//
//				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
//				SDL_RenderClear(renderer);
//
//				SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
//				SDL_RenderDrawLine(renderer, 320, 200, 300, 240);
//				SDL_RenderDrawLine(renderer, 300, 240, 340, 240);
//				SDL_RenderDrawLine(renderer, 340, 240, 320, 200);
//				SDL_RenderPresent(renderer);
//
//				while (SDL_PollEvent(&event)) {
//					if (event.type == SDL_QUIT) {
//						done = SDL_TRUE;
//					}
//				}
//			}
//		}
//
//		if (renderer) {
//			SDL_DestroyRenderer(renderer);
//		}
//		if (window) {
//			SDL_DestroyWindow(window);
//		}
//	}
//	SDL_Quit();
//	return 0;
//}