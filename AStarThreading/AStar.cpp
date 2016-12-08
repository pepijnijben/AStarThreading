#include "AStar.h"
#include "Game.h"

Node* AStar::GetNode(Point2D pos, bool create)
{
	if (nodeMap.find(pos.ToString()) != nodeMap.end())
	{
		return nodeMap[pos.ToString()];
	}
	if (!create)
		throw std::exception("No such element found");

	Node* node = new Node(pos);
	nodeMap[pos.ToString()] = node;

	return node;
}

void AStar::ClearAll()
{
	for (std::map<std::string, Node*>::iterator itr = nodeMap.begin(); itr != nodeMap.end(); itr++)
	{
		itr->second->Reset();
	}
}

void AStar::DestroyAll()
{
	for (std::map<std::string, Node*>::iterator itr = nodeMap.begin(); itr != nodeMap.end(); itr++)
	{
		delete itr->second;
	}

	nodeMap.clear();
}

void AStar::DefineGraph(int tileCount, int tileSize, int numberOfWalls)
{
	DestroyAll();

	Point2D current;
	Point2D addXVector(tileSize, 0);
	Point2D addYVector(0, tileSize);

	float middleOffset = tileSize * 0.5f;

	int wallEvery = tileCount / numberOfWalls;
	int wallAtX = (tileCount / numberOfWalls) * 0.5f;
	int currentWallCounter = 0;

	for (int x = 0; x < tileCount; x++)
	{
		if (x % wallEvery == wallAtX)
		{
			currentWallCounter++;
		}
		current.x = (x * tileSize) + middleOffset;
		bool isRight = (x + 1) == tileCount;

		for (int y = 0; y < tileCount; y++)
		{
			if (x % wallEvery == wallAtX)
			{
				if ((currentWallCounter % 2 == 0 && y != 0)
					|| (currentWallCounter % 2 == 1 && y != tileCount - 1))
				{
					continue;
				}
			}

			current.y = (y * tileSize) + middleOffset;

			bool isBottom = (y + 1) == tileCount;

			bool emptyRight = !isRight && !(((x + 1) % wallEvery == wallAtX) 
				&& ((currentWallCounter % 2 == 0 && y != tileCount - 1) || (currentWallCounter % 2 == 1 && y != 0)));
			bool emptyBelow = !isBottom && !(currentWallCounter % 2 == 0 && y == 0 && x % wallEvery == wallAtX);

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
		std::cout << "Could not find this element" << std::endl;
		return std::vector<Point2D>();
	}

	Node * node = nodeMap[currentNode.ToString()];

	std::vector<Point2D> points;
	for(auto obj : node->adjacent)
	{
		points.push_back(obj.destination->pos);
	}

	return points;
}

std::vector<Point2D> AStar::PathFromTo(Point2D from, Point2D to)
{
	std::cout << "Finding path from " << from.ToString() << " to " << to.ToString() << std::endl;

	ClearAll();

	if (!NodeExists(from) || !NodeExists(to))
	{
		std::cout << "Could not find the from/to nodes, canceling A*" << std::endl;
		return std::vector<Point2D>();
	}

	if (from == to)
		return std::vector<Point2D>();
	std::priority_queue<AStarValue> pq;
	bool test = false;
	pq.push(AStarValue(nodeMap[from.ToString()], 0, ManhattanDistance(from, to)));
	while (!pq.empty())
	{
		AStarValue aStarNode = pq.top();
		Node * node = aStarNode.node;
		pq.pop();

		if (node->state == NodeState::Closed)
		{
			continue;
		}
		node->state = NodeState::Closed;

		for(auto adj : node->adjacent)
		{
			if (adj.destination->pos == to)
			{
				adj.destination->parent = node;

				return BacktrackRoute(adj.destination);
			}
			if (adj.destination->state == NodeState::Closed)
			{
				continue;
			}

			float newDistance = aStarNode.Travelled + adj.cost;

			if (adj.destination->state == NodeState::Open) // Seen before but not closed yet
			{
				if (newDistance < adj.destination->distance)
				{
					adj.destination->parent = node;
					adj.destination->distance = newDistance;
					pq.push(AStarValue(adj.destination, newDistance, ManhattanDistance(adj.destination->pos, to)));
				}
			}
			else
			{
				adj.destination->parent = node;
				adj.destination->distance = newDistance;
				adj.destination->state = NodeState::Open;
				pq.push(AStarValue(adj.destination, newDistance, ManhattanDistance(adj.destination->pos, to)));
			}
		}
	}

	std::cout << "Found path from " << from.ToString() << " to " << to.ToString() << std::endl;

	return std::vector<Point2D>();
}

std::vector<Point2D> AStar::PathFromTo(int fx, int fy, int tx, int ty)
{
	float offset = Game::TileSize / 2;
	return PathFromTo(Point2D(fx * Game::TileSize + offset, fy * Game::TileSize + offset), Point2D(tx * Game::TileSize + offset, ty * Game::TileSize + offset));
}

std::vector<Point2D> AStar::BacktrackRoute(Node * end)
{
	std::vector<Point2D> route;
	Node * routeNode = end;
	while (routeNode != nullptr)
	{
		route.insert(route.begin(), routeNode->pos);
		routeNode = routeNode->parent;
	}

	return route;
}
