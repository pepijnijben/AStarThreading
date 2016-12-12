#include "stdafx.h"
#include <iostream>
#include "LTimer.h"
#include "Game.h"

using namespace std;

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

int Game::TileSize = 20;
int Game::TileCount = 30;
int Game::WallCount = 3;
Point2D Game::m_camPos = Point2D(0,0);

Game::Game()
{
	pause = false;
	quit = false;
}

Game::~Game()
{
}


void Game::GetPath(Enemy * e, Point2D from, Point2D to)
{
	vector<Point2D> path = aStar.PathFromTo(from.x, from.y, to.x, to.y);
	e->SetPath(path);
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

	// Generate map
	TileCount = tileCount[currentSimulation];
	WallCount = numberOfWalls[currentSimulation];

	int wallEvery = TileCount / WallCount;
	int wallAtX = (TileCount / WallCount) * 0.5f;
	int currentWallCounter = 0;

	for (int x = 0; x < TileCount; x ++)
	{
		int coll = x % 2;
		if (x % wallEvery == wallAtX)
		{
			currentWallCounter++;
		}
		for (int y = 0; y < TileCount; y++)
		{
			coll++;
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

			if (coll % 2 == 0)
			{
				gameObjects.push_back(new Tile(x * TileSize, y * TileSize, TileSize, Colour(150, 150, 150)));
			}
			else
			{
				gameObjects.push_back(new Tile(x * TileSize, y * TileSize, TileSize, Colour(66, 66, 66)));
			}
		}
	}
	// End generate map

	// Generate waypoints


	// Generate enemies
	for (int i = 0; i < enemyCount[currentSimulation]; i++)
	{
		float x = ((TileCount - 4) + i % 4);
		float y = (0 + (i / 4));
		Enemy * e = new Enemy(x * TileSize, y * TileSize, TileSize);
		m_enemies.push_back(e);
		//threadPool.AddJob(bind(&Game::GetPath, this, e, Point2D(x, y), Point2D()));
	}
	// End generate enemies

	return true;
}

void Game::destroy()
{
	//empty out the game object vector before quitting
	for (std::vector<Tile*>::iterator i = gameObjects.begin(); i != gameObjects.end(); i++) {
		delete *i;
	}
	for (std::vector<Enemy*>::iterator i = m_enemies.begin(); i != m_enemies.end(); i++) {
		delete *i;
	}
	gameObjects.clear();
	m_enemies.clear();
	renderer.destroy();
}

//** calls update on all game entities*/
void Game::update()
{
	unsigned int currentTime = LTimer::gameTime();//millis since game started
	unsigned int deltaTime = currentTime - lastTime;//time since last update

	for (std::vector<Enemy*>::iterator i = m_enemies.begin(); i != m_enemies.end(); i++) {
		(*i)->Update(deltaTime);
	}

	//save the curent time for next frame
	lastTime = currentTime;
}

//** calls render on all game entities*/
void Game::render()
{
	renderer.clear(Colour(0,0,0));// prepare for new frame
	
	int xPos = (m_camPos.x / TileSize) * -1;
	int yPos = (m_camPos.y / TileSize) * -1;
	int size = gameObjects.size();

	for(int x = xPos; x < xPos + 40; x++)
	{
		for (int y = yPos; y < yPos + 30; y++)
		{
			int index = (x * TileCount) + y;
			if (index < size && index >= 0)
			{
				gameObjects[index]->Render(renderer);
			}
		}
	}

	//render every object
	/*for (std::vector<Tile*>::iterator i = gameObjects.begin(), e= gameObjects.end(); i != e; i++) {
		(*i)->Render(renderer);
	}*/

	for (std::vector<Enemy*>::iterator i = m_enemies.begin(), e = m_enemies.end(); i != e; i++) {
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
		m_camPos.x -= TileSize;
	}
	if (evt == EventListener::Event::LEFTARROW)
	{
		m_camPos.x += TileSize;
	}
	if (evt == EventListener::Event::UPARROW)
	{
		m_camPos.y += TileSize;
	}
	if (evt == EventListener::Event::DOWNARROW)
	{
		m_camPos.y -= TileSize;
	}
}
