#pragma once
#include <map>
#include <queue>
#include "BasicTypes.h"

class AStar
{
private:
	std::map<int, std::map<std::string, Node*>> nodeMaps;

	Node* GetNode(std::map<std::string, Node*> * map, Point2D pos, bool create = false);
	void ClearAll(int thread_id);
	void DestroyAll(std::map<std::string, Node*> * map);

	std::vector<Point2D> BacktrackRoute(Node * end);
	float ManhattanDistance(Point2D from, Point2D to) { return abs(from.x - to.x) + abs(from.y - to.y); };
public:
	AStar() {};
	~AStar() { /*DestroyAll();*/ };

	std::map<std::string, Node*> DefineGraph();
	void AddEdge(std::map<std::string, Node*> * map, Point2D from, Point2D to, float cost = 1.0f, bool omniDirection = true);
	std::vector<Point2D> GetEdges(std::map<std::string, Node*> * map, Point2D currentNode);
	bool NodeExists(std::map<std::string, Node*> * map, Point2D node) { return map->find(node.ToString()) != map->end(); };
	std::vector<Point2D> PathFromTo(Point2D from, Point2D to);
	std::vector<Point2D> PathFromTo(int fx, int fy, int tx, int ty);
};
