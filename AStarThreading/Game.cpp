#include "stdafx.h"
#include <iostream>
#include "LTimer.h"
#include "Game.h"

using namespace std;

int SCREEN_FPS = 60;
int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

void SET_MAX_FPS(int fps)
{
	SCREEN_FPS = fps;
	SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
}

int Game::TileSize = 20;
int Game::TileCount = 30;
int Game::WallCount = 3;
SDL_Texture* Game::spritesheet = nullptr;
Point2D Game::m_camPos = Point2D(0,0);
vector<Tile> Game::gameObjects;
vector<Point2D> Game::m_waypoints;
vector<vector<Point2D> *> Game::m_waypointPaths;

Game::Game()
{
	quit = false;
	aStarStarted = false;
	followLeader = false;
	followPlayer = false;
	wayPointsDone = false;
	finnished = false;
	playerUpdate = 0;
	m_wayPointsLock = SDL_CreateMutex();

	////////////////////////////////////
	//////////// Settings //////////////
	////////////////////////////////////
	currentSimulation = 0; // 0; 1 or 2
	useThreadPool = true;
	SET_MAX_FPS(60); // Over 1000 no max fps
	////////////////////////////////////

	if (currentSimulation == 0)
	{
		m_camPos.x = 100;
	}

	srand(time(NULL));
}

Game::~Game()
{
}


void Game::GetPath(Enemy * e, Point2D from, Point2D to)
{
	vector<Point2D> path = aStar.PathFromTo(from.x, from.y, to.x, to.y);
	e->SetPath(path);
}

void Game::GetWaypointPath(Point2D from, Point2D to)
{
	vector<Point2D> path = aStar.PathFromTo(from, to);

	SDL_LockMutex(m_wayPointsLock);
	m_waypointPaths.push_back(new vector<Point2D>(path));
	SDL_UnlockMutex(m_wayPointsLock);
}

bool Game::init() {
	Size2D winSize(800, 600);

	//creates our renderer, which looks after drawing and the window
	renderer.init(winSize,"AStarThreading");

	// Load sprite sheet
	SDL_Surface* Loading_Surf = SDL_LoadBMP("spritesheet.bmp");
	SDL_SetColorKey(Loading_Surf, SDL_TRUE, SDL_MapRGB(Loading_Surf->format, 255, 0, 255));
	spritesheet = SDL_CreateTextureFromSurface(renderer.getSDLRenderer(), Loading_Surf);
	SDL_FreeSurface(Loading_Surf);

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
	inputManager.AddListener(EventListener::Event::FOLLOW, this);
	inputManager.AddListener(EventListener::Event::FOLLOWPLAYER, this);
	inputManager.AddListener(EventListener::Event::SPACE, this);
	inputManager.AddListener(EventListener::Event::QUIT, this);
	inputManager.AddListener(EventListener::Event::RIGHTARROW, this);
	inputManager.AddListener(EventListener::Event::LEFTARROW, this);
	inputManager.AddListener(EventListener::Event::UPARROW, this);
	inputManager.AddListener(EventListener::Event::DOWNARROW, this);
	inputManager.AddListener(EventListener::Event::RESPAWN, this);

	// Generate map
	TileCount = tileCount[currentSimulation];
	WallCount = numberOfWalls[currentSimulation];

	int wallEvery = TileCount / WallCount;
	int wallAtX = (TileCount / WallCount) * 0.5f;
	int currentWallCounter = 0;

	for (int x = 0; x < TileCount; x ++)
	{
		int coll = x % 3;
		if (x % wallEvery == wallAtX)
		{
			currentWallCounter++;
		}
		for (int y = 0; y < TileCount; y++)
		{
			coll++;
			if (x % wallEvery == wallAtX)
			{
				if ((currentWallCounter % 2 == 0 && y != 0)
					|| (currentWallCounter % 2 == 1) && y != TileCount - 1)
				{
					gameObjects.push_back(Tile(x * TileSize, y * TileSize, TileSize, 3, NodeState::Wall));
					continue;
				}
				m_waypoints.push_back(Point2D(x * TileSize, y * TileSize));
			}

			gameObjects.push_back(Tile(x * TileSize, y * TileSize, TileSize, coll % 3));
		}
	}
	// End generate map
	m_playerPos = Point2D(2 * TileSize, (TileCount / 2) * TileSize);

	cout << "Started calculating waypoints" << endl;
	// Calculate waypointspath
	for (int i = m_waypoints.size() - 1; i >= 0; i--)
	{
		if (i != 0)
		{
			// switch for using threadpool or not
			useThreadPool ? threadPool.AddJob(bind(&Game::GetWaypointPath, this, m_waypoints[i], m_waypoints[i - 1])) : GetWaypointPath(m_waypoints[i], m_waypoints[i - 1]);
		}
	}
	// Add waypoint to start position of player
	// switch for using threadpool or not
	useThreadPool ? threadPool.AddJob(bind(&Game::GetWaypointPath, this, m_waypoints[0], m_playerPos)) : GetWaypointPath(m_waypoints[0], m_playerPos);

	// Generate enemies
	RespawnEnemies();
	// End generate enemies

	return true;
}

void Game::destroy()
{
	for (std::vector<Enemy*>::iterator i = m_enemies.begin(); i != m_enemies.end(); i++) {
		delete *i;
	}
	gameObjects.clear();
	m_enemies.clear();
	renderer.destroy();

	cout << "Cleaned everything up" << endl;
}

//** calls update on all game entities*/
void Game::update()
{
	// Update Player every half second
	if (aStarStarted && playerUpdate + 3000 < SDL_GetTicks())
	{
		playerUpdate = SDL_GetTicks();
		int direction = rand() % 4;
		Point2D moving;

		switch (direction)
		{
		case 0:
			moving.x += TileSize;
			break;
		case 1:
			moving.x -= TileSize;
			break;
		case 2:
			moving.y += TileSize;
			break;
		case 3:
			moving.y -= TileSize;
			break;
		}

		m_playerPos = m_playerPos + moving;
		if (m_playerPos.x < 0 || m_playerPos.y < 0 || m_playerPos.x > TileCount * TileSize || m_playerPos.y > TileCount * TileSize)
		{
			m_playerPos = m_playerPos - moving;
		}
	}
	// End Update Player

	if (!wayPointsDone)
	{
		SDL_LockMutex(m_wayPointsLock);
		int size = m_waypointPaths.size();
		SDL_UnlockMutex(m_wayPointsLock);

		if (size + 1 >= WallCount)
		{
			wayPointsDone = true;
			cout << "Waypoints calculated!" << endl;
		}
	}

	if (followLeader)
	{
		if (m_enemies.size() < enemyCount[currentSimulation])
		{
			m_camPos.x = m_playerPos.x * -1 + 400;
			m_camPos.y = m_playerPos.y * -1 + 300;
		}
		else
		{
			m_camPos.x = m_enemies[0]->GetPos().x * -1 + 400;
			m_camPos.y = m_enemies[0]->GetPos().y * -1 + 300;
		}
	}
	else if (followPlayer)
	{
		m_camPos.x = m_playerPos.x * -1 + 400;
		m_camPos.y = m_playerPos.y * -1 + 300;
	}

	unsigned int currentTime = LTimer::gameTime();//millis since game started
	unsigned int deltaTime = currentTime - lastTime;//time since last update
	for (std::vector<Enemy*>::iterator i = m_enemies.begin(); i != m_enemies.end();) {
		(*i)->Update(deltaTime);
		if (aStarStarted && !(*i)->FindingPath() && (*i)->PathDone() && (*i)->GetPos() != m_playerPos)
		{
			(*i)->FindingPath(true);
			for (int c = 0; c < m_waypoints.size(); c++)
			{
				if (m_waypoints[c] == (*i)->GetPos())
				{
					// return waypiont path
					for (auto & path : Game::m_waypointPaths)
					{
						if (path != nullptr && (*i)->GetPos() == path->at(0))
						{
							(*i)->SetPath(*(path));
							break;
						}
					}
				}
			}
			
			// Only add pathfinding when no waypoint paths are found
			if ((*i)->FindingPath())
			{
				// switch for using threadpool or not
				!useThreadPool ? GetPath((*i), Point2D((*i)->GetPos().x / TileSize, (*i)->GetPos().y / TileSize), Point2D(m_playerPos.x / TileSize, m_playerPos.y / TileSize)):
				threadPool.AddJob(bind(&Game::GetPath, this, (*i), Point2D((*i)->GetPos().x / TileSize, (*i)->GetPos().y / TileSize), Point2D(m_playerPos.x / TileSize, m_playerPos.y / TileSize)));
			}
		}
		else if ((*i)->GetPos() == m_playerPos)
		{
			delete * i;
			i = m_enemies.erase(i);
			continue;
		}

		++i;
	}

	// Check if all enemies are done
	if (!finnished && 0 == m_enemies.size())
	{
		aStarStarted = false;
		finnished = true;
		cout << "Clock ticks: " << SDL_GetTicks() - startTime << endl;
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
				gameObjects[index].Render(renderer);
			}
		}
	}

	for (std::vector<Enemy*>::iterator i = m_enemies.begin(), e = m_enemies.end(); i != e; i++) {
		(*i)->Render(renderer);
	}

	// Draw player
	renderer.drawSprite(Rect(m_playerPos, Size2D(TileSize, TileSize)), 4);

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

		update();
		render();

		int frameTicks = capTimer.getTicks();//time since start of frame
		if (frameTicks < SCREEN_TICKS_PER_FRAME && SCREEN_TICKS_PER_FRAME - frameTicks > 0)
		{
			//Wait remaining time before going to next frame
			SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
		}
	}
}

void Game::onEvent(EventListener::Event evt) {
	
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

	if (evt==EventListener::Event::SPACE)
	{
		if (!wayPointsDone)
		{
			cout << "Wait for waypoints to be done!" << endl;
		}
		else
		{
			StartAStar();
		}
	}

	if (evt == EventListener::Event::FOLLOW)
	{
		followLeader = !followLeader;
		followPlayer = false;
	}

	if (evt == EventListener::Event::FOLLOWPLAYER)
	{
		followPlayer = !followPlayer;
		followLeader = false;
	}

	if (evt == EventListener::Event::RESPAWN)
	{
		RespawnEnemies();
	}
}

void Game::StartAStar()
{
	if (!aStarStarted)
	{
		playerUpdate = SDL_GetTicks();
		startTime = SDL_GetTicks();
		aStarStarted = true;
		finnished = false;

		for(auto & enemy : m_enemies)
		{
			Point2D pos = enemy->GetPos();
			int x = pos.x / TileSize;
			int y = pos.y / TileSize;

			enemy->FindingPath(true);
			
			// switch for using threadpool or not
			!useThreadPool ? GetPath(enemy, Point2D(x, y), Point2D(m_playerPos.x / TileSize, m_playerPos.y / TileSize)) :
			threadPool.AddJob(bind(&Game::GetPath, this, enemy, Point2D(x, y), Point2D(m_playerPos.x / TileSize, m_playerPos.y / TileSize)));
		}

		cout << "Started A*" << endl;
	}
}

void Game::RespawnEnemies()
{
	m_playerPos = Point2D(2 * TileSize, (TileCount / 2) * TileSize);
	if (m_enemies.size() > 0)
	{
		cout << "Cant spawn enemies there are still some running around!" << endl;
		return;
	}

	for (int i = 0; i < enemyCount[currentSimulation]; i++)
	{
		float x = ((TileCount - 4) + i % 4);
		float y = (0 + (i / 4));
		Enemy * e = new Enemy(x * TileSize, y * TileSize, TileSize);
		m_enemies.push_back(e);
	}
}
