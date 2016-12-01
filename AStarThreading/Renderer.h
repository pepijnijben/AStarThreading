#pragma once
#define SDL_MAIN_HANDLED
#ifdef __APPLE__
#include "SDL2/SDL.h"
#elif defined(_WIN64) || defined(_WIN32)
#include "SDL.h"
#endif


#include "BasicTypes.h"


//Responsible for all drawing operations
//abstracts away specfic SDL specific drawing functions
class Renderer{

	// size of window in pixels
	Size2D windowSize;
	//position of window in world coordinates
	//change these if you want to zoom or pan
	Point2D viewportBottomLeft;
	Size2D viewportSize;


	SDL_Window *window;
	SDL_Renderer *sdl_renderer;

public:
	Renderer();
	bool init(const Size2D&, const char*);
	void drawRect(const Rect&, const Colour&);
	void drawWorldRect(const Rect&, const Colour&);
	void present();
	void clear(const Colour&);
	Point2D worldToScreen(const Point2D&);
	Rect worldToScreen(const Rect&);

	void setViewPort(const Rect&);

	void destroy();
	~Renderer();
};

