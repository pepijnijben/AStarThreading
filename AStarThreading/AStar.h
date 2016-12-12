#pragma once
#include <map>
#include <queue>
#include "BasicTypes.h"
#include "Tile.h"

class AStarValue
{
public:
	float Travelled;
	float DistanceToTarget;
	Tile * node;

	AStarValue(Tile * n, float t, float d)
	{
		node = n;
		Travelled = t;
		DistanceToTarget = d;
	}

	bool operator<(const AStarValue& lhs) const
	{
		return (lhs.Travelled + lhs.DistanceToTarget) < (Travelled + DistanceToTarget);
	}
};

class AStar
{
private:
	std::vector<Point2D> BacktrackRoute(Tile * end);
	float ManhattanDistance(Point2D from, Point2D to) { return abs(from.x - to.x) + abs(from.y - to.y); };
public:
	AStar() {};
	~AStar() {};

	bool NodeExists(std::map<std::string, Node*> * map, Point2D node) { return map->find(node.ToString()) != map->end(); };
	std::vector<Point2D> PathFromTo(Point2D from, Point2D to);
	std::vector<Point2D> PathFromTo(int fx, int fy, int tx, int ty);
};
