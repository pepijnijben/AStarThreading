#pragma once
#include "BasicTypes.h"
#include <map>
#include <queue>
#include "Game.h"

class AStar
{
private:
	std::map<std::string, Node*> nodeMap;
	Node* GetNode(Point2D pos, bool create = false);
	void ClearAll();
	void DestroyAll();

	std::vector<Point2D> BacktrackRoute(Node * end);
	float ManhattanDistance(Point2D from, Point2D to) { return abs(from.x - to.x) + abs(from.y - to.y); };
public:
	AStar() {};
	~AStar() { DestroyAll(); };

	void DefineGraph(int tileCount, int tileSize, int numberOffWalls);
	void AddEdge(Point2D from, Point2D to, float cost = 1.0f, bool omniDirection = true);
	std::vector<Point2D> GetEdges(Point2D currentNode);
	bool NodeExists(Point2D node) { return nodeMap.find(node.ToString()) != nodeMap.end(); };
	std::vector<Point2D> PathFromTo(Point2D from, Point2D to);
	std::vector<Point2D> PathFromTo(int fx, int fy, int tx, int ty)
	{
		float offset = Game::TileSize / 2;
		return PathFromTo(Point2D(fx * Game::TileSize + offset, fy * Game::TileSize + offset), Point2D(tx * Game::TileSize + offset, ty * Game::TileSize + offset));
	};
};
