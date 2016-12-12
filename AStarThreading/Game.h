#pragma once
#include <vector>
#include "Renderer.h"
#include "GameObject.h"
#include "InputManager.h"
#include "EventListener.h"
#include "ThreadPool.h"
#include "AStar.h"
#include "Enemy.h"
#include "Tile.h"

/** The game objct whic manages the game loop*/
class Game:public EventListener
{
	InputManager inputManager;
	Renderer renderer;
	ThreadPool threadPool;

	std::vector<Tile> gameObjects;
	std::vector<Enemy*> m_enemies;
	AStar aStar;

	unsigned int lastTime;//time of last update;

	bool quit;
	bool aStarStarted;
	bool followLeader;

	int currentSimulation = 1;
	const int tileCount[3] = { 30, 100, 1000 };
	const int numberOfWalls[3] = { 3, 6, 12 };
	const int enemyCount[3] = { 5, 50, 1 };

	void StartAStar();

public:
	Game();
	~Game();

	void GetPath(vector<Tile> map, Enemy * e, Point2D from, Point2D to);
	bool init();
	void destroy();

	void update();
	void render();
	void loop();

	void onEvent(EventListener::Event);

	static Point2D m_camPos;
	static int TileSize;
	static int TileCount;
	static int WallCount;
};

