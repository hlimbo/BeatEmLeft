#ifndef CORE_H
#define CORE_H

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480

struct SDL_Window;
struct SDL_Renderer;

class Core
{
public:
	Core();
	~Core();
public:
	int getTargetFPS();
	float getTargetDeltaTime();
	SDL_Renderer* getRenderer();
	SDL_Window* getWindow();
private:
	//Initializes SDL Subsystems, creates game window, and renderer.
	//returns true on successful initialization; false otherwise.
	bool appInitialization();
	//Destroys SDL Components used and shuts down sdl subsystems used.
	void appCleanUp();
private:
	SDL_Window* mainWindow;
	SDL_Renderer* mainRenderer;
	int targetFPS;
	float targetDeltaTime;//measured in ms
};

#endif

