#include "stdafx.h"
#include <iostream>
using namespace std;

#include "LTimer.h"
#include "Game.h"
#include "AStar.h"
#include "Tile.h"

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

int Game::TileSize = 20;
int Game::TileCount = 30;
Point2D Game::m_camPos = Point2D(0,0);

Game::Game()
{
	pause = false;
	quit = false;
}

Game::~Game()
{
}

bool Game::init() {	
	Size2D winSize(800, 600);

	//creates our renderer, which looks after drawing and the window
	renderer.init(winSize,"AStarThreading");

	//set up the viewport
	//we want the vp centred on origin and 20 units wide
	float aspectRatio = winSize.w / winSize.h;
	float vpWidth = 20;
	Size2D vpSize(vpWidth, vpWidth /aspectRatio);
	Point2D vpBottomLeft( -vpSize.w / 2, - vpSize.h / 2);

	Rect vpRect(vpBottomLeft,vpSize);
	renderer.setViewPort(vpRect);
	
	lastTime = LTimer::gameTime();

	//want game loop to pause
	inputManager.AddListener(EventListener::Event::PAUSE, this);
	inputManager.AddListener(EventListener::Event::QUIT, this);
	inputManager.AddListener(EventListener::Event::RIGHTARROW, this);
	inputManager.AddListener(EventListener::Event::LEFTARROW, this);
	inputManager.AddListener(EventListener::Event::UPARROW, this);
	inputManager.AddListener(EventListener::Event::DOWNARROW, this);

	int numberOfWalls = 3;
	int wallEvery = TileCount / numberOfWalls;
	int wallAtX = (TileCount / numberOfWalls) * 0.5f;
	int currentWallCounter = 0;

	for (int x = 0; x < TileCount; x ++)
	{
		if (x % wallEvery == wallAtX)
		{
			currentWallCounter++;
		}
		for (int y = 0; y < TileCount; y++)
		{
			if (x % wallEvery == wallAtX)
			{
				if (x % wallEvery == wallAtX)
				{
					if ((currentWallCounter % 2 == 0 && y != 0)
						|| (currentWallCounter % 2 == 1) && y != TileCount - 1)
					{
						gameObjects.push_back(new Tile(x * TileSize, y * TileSize, TileSize, Colour(255, 0, 0)));
						continue;
					}
				}
			}

			gameObjects.push_back(new Tile(x * TileSize, y * TileSize, TileSize, Colour(0, 255, 0)));
		}
	}

	AStar aStar;
	aStar.DefineGraph(TileCount, TileSize);

	vector<Point2D> path = aStar.PathFromTo(0, 0, 29, 29);

	for(Point2D point : path)
	{
		gameObjects.push_back(new Tile(point.x - (TileSize * 0.5f), point.y - (TileSize * 0.5f), 20, Colour(0, 0, 255)));
	}

	return true;
}

void Game::destroy()
{
	//empty out the game object vector before quitting
	for (std::vector<GameObject*>::iterator i = gameObjects.begin(); i != gameObjects.end(); i++) {
		delete *i;
	}
	gameObjects.clear();
	renderer.destroy();
}

//** calls update on all game entities*/
void Game::update()
{
	unsigned int currentTime = LTimer::gameTime();//millis since game started
	unsigned int deltaTime = currentTime - lastTime;//time since last update

	//call update on all game objects
	for (std::vector<GameObject*>::iterator i = gameObjects.begin(); i != gameObjects.end(); i++) {
		(*i)->Update(deltaTime);
	}

	//save the curent time for next frame
	lastTime = currentTime;
}

//** calls render on all game entities*/
void Game::render()
{
	renderer.clear(Colour(0,0,0));// prepare for new frame
	
	//render every object
	for (std::vector<GameObject*>::iterator i = gameObjects.begin(), e= gameObjects.end(); i != e; i++) {
		(*i)->Render(renderer);
	}

	renderer.present();// display the new frame (swap buffers)
}

/** update and render game entities*/
void Game::loop()
{
	LTimer capTimer;//to cap framerate

	int frameNum = 0;
	while (!quit) { //game loop
		capTimer.start();

		inputManager.ProcessInput();

		if(!pause) //in pause mode don't bother updateing
			update();
		render();

		int frameTicks = capTimer.getTicks();//time since start of frame
		if (frameTicks < SCREEN_TICKS_PER_FRAME)
		{
			//Wait remaining time before going to next frame
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
		}
	}
}

void Game::onEvent(EventListener::Event evt) {

	if (evt == EventListener::Event::PAUSE) {
		pause = !pause;
	}
	
	if (evt == EventListener::Event::QUIT) {
		quit=true;
	}

	if (evt == EventListener::Event::RIGHTARROW)
	{
		m_camPos.x -= 15.0f;
	}
	if (evt == EventListener::Event::LEFTARROW)
	{
		m_camPos.x += 15.0f;
	}
	if (evt == EventListener::Event::UPARROW)
	{
		m_camPos.y += 15.0f;
	}
	if (evt == EventListener::Event::DOWNARROW)
	{
		m_camPos.y -= 15.0f;
	}
}
