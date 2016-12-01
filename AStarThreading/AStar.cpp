#include "AStar.h"

Node* AStar::GetNode(Point2D pos, bool create)
{
	if (nodeMap.find(pos) != nodeMap.end())
	{
		return nodeMap[pos];
	}
	if (!create)
		throw std::exception("No such element found");

	Node* node = new Node(pos);
	nodeMap[pos] = node;

	return node;
}

void AStar::ClearAll()
{
	for (std::map<Point2D, Node*>::iterator itr = nodeMap.begin(); itr != nodeMap.end(); itr++)
	{
		itr->second->Reset();
	}
}

void AStar::DestroyAll()
{
	for (std::map<Point2D, Node*>::iterator itr = nodeMap.begin(); itr != nodeMap.end(); itr++)
	{
		delete itr->second;
	}

	nodeMap.clear();
}

void AStar::DefineGraph(int tileCount, int tileSize)
{
	Point2D current;
	Point2D addXVector(tileSize, 0);
	Point2D addYVector(0, tileSize);
	float lengthDiagonal = sqrt(pow(tileSize, 2) * 2);

	int middleOffset = tileSize * 0.5f;

	for (int x = 0; x < tileCount; x++)
	{
		current.x = x * tileSize + middleOffset;
		bool isRight = (x + 1) == tileCount;

		for (int y = 0; y < tileCount; y++)
		{
			current.y = y * tileSize + middleOffset;

			bool isBottom = (y + 1) == tileCount;

			bool emptyRight = !isRight;
			bool emptyBelow = !isBottom;

			if (emptyRight)
			{
				AddEdge(current, current + addXVector);
			}
			if (emptyBelow)
			{
				AddEdge(current, current + addYVector);
			}
		}
	}
}

void AStar::AddEdge(Point2D from, Point2D to, float cost, bool omniDirection)
{
	Node * v = GetNode(from, true);
	Node * w = GetNode(to, true);

	v->adjacent.push_back(Edge(w, cost));
	if (omniDirection)
	{
		w->adjacent.push_back(Edge(v, cost));
	}
}

std::vector<Point2D> AStar::GetEdges(Point2D currentNode)
{
	if (!NodeExists(currentNode))
	{
		throw std::exception("No such element found");
	}

	Node * node = nodeMap[currentNode];

	std::vector<Point2D> points;
	for(auto obj : node->adjacent)
	{
		points.push_back(obj.destination->pos);
	}

	return points;
}
