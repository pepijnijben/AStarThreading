#pragma once
#include "BasicTypes.h"
#include <map>

class AStar
{
private:
	std::map<Point2D, Node*> nodeMap;
	Node* GetNode(Point2D pos, bool create = false);
	void ClearAll();
	void DestroyAll();

	float ManhattanDistance(Point2D from, Point2D to) { return abs(from.x - to.x) + abs(from.y - to.y); };
public:
	void DefineGraph(int tileCount, int tileSize);
	void AddEdge(Point2D from, Point2D to, float cost = 1.0f, bool omniDirection = true);
	std::vector<Point2D> GetEdges(Point2D currentNode);
	bool NodeExists(Point2D node) { return nodeMap.find(node) != nodeMap.end(); };
};
