#include "AStar.h"
#include "Game.h"

//Node* AStar::GetNode(Point2D pos, bool create)
//{
//	if (nodeMap.find(pos.ToString()) != nodeMap.end())
//	{
//		return nodeMap[pos.ToString()];
//	}
//	if (!create)
//		throw std::exception("No such element found");
//
//	Node* node = new Node(pos);
//	nodeMap[pos.ToString()] = node;
//
//	return node;
//}

Node * AStar::GetNode(std::map<std::string, Node*> * map, Point2D pos, bool create)
{
	if (map->find(pos.ToString()) != map->end())
	{
		return (*map)[pos.ToString()];
	}
	if (!create)
		throw std::exception("No such element found");

	Node* node = new Node(pos);
	(*map)[pos.ToString()] = node;

	return node;
}

void AStar::DestroyAll(std::map<std::string, Node*> * map)
{
	for (std::map<std::string, Node*>::iterator itr = map->begin(); itr != map->end(); itr++)
	{
		delete itr->second;
	}

	map->clear();
}

std::map<std::string, Node*> AStar::DefineGraph()
{
	std::map<std::string, Node*> nodeMap;

	Point2D current;
	Point2D addXVector(Game::TileSize, 0);
	Point2D addYVector(0, Game::TileSize);

	float middleOffset = 0;
	//float middleOffset = tileSize * 0.5f;

	int wallEvery = Game::TileCount / Game::WallCount;
	int wallAtX = (Game::TileCount / Game::WallCount) * 0.5f;
	int currentWallCounter = 0;

	for (int x = 0; x < Game::TileCount; x++)
	{
		if (x % wallEvery == wallAtX)
		{
			currentWallCounter++;
		}
		current.x = (x * Game::TileSize) + middleOffset;
		bool isRight = (x + 1) == Game::TileCount;

		for (int y = 0; y < Game::TileCount; y++)
		{
			if (x % wallEvery == wallAtX)
			{
				if ((currentWallCounter % 2 == 0 && y != 0)
					|| (currentWallCounter % 2 == 1 && y != Game::TileCount - 1))
				{
					continue;
				}
			}

			current.y = (y * Game::TileSize) + middleOffset;

			bool isBottom = (y + 1) == Game::TileCount;

			bool emptyRight = !isRight && !(((x + 1) % wallEvery == wallAtX)
				&& ((currentWallCounter % 2 == 0 && y != Game::TileCount - 1) || (currentWallCounter % 2 == 1 && y != 0)));
			bool emptyBelow = !isBottom && !(currentWallCounter % 2 == 0 && y == 0 && x % wallEvery == wallAtX);

			if (emptyRight)
			{
				AddEdge(&nodeMap, current, current + addXVector);
			}
			if (emptyBelow)
			{
				AddEdge(&nodeMap, current, current + addYVector);
			}
		}
	}

	return nodeMap;
}

void AStar::AddEdge(std::map<std::string, Node*> * map, Point2D from, Point2D to, float cost, bool omniDirection)
{
	Node * v = GetNode(map, from, true);
	Node * w = GetNode(map, to, true);
	
	v->adjacent.push_back(Edge(w, cost));
	if (omniDirection)
	{
		w->adjacent.push_back(Edge(v, cost));
	}
}

std::vector<Point2D> AStar::GetEdges(std::map<std::string, Node*> * map, Point2D currentNode)
{
	if (!NodeExists(map, currentNode))
	{
		std::cout << "Could not find this element" << std::endl;
		return std::vector<Point2D>();
	}

	Node * node = (*map)[currentNode.ToString()];

	std::vector<Point2D> points;
	for(auto obj : node->adjacent)
	{
		points.push_back(obj.destination->pos);
	}

	return points;
}

std::vector<Point2D> AStar::PathFromTo(Point2D from, Point2D to)
{
	std::map<std::string, Node*> nodeMap = DefineGraph();

	if (!NodeExists(&nodeMap, from) || !NodeExists(&nodeMap, to))
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

	//std::cout << "Found path from " << from.ToString() << " to " << to.ToString() << std::endl;

	return std::vector<Point2D>();
}

std::vector<Point2D> AStar::PathFromTo(int fx, int fy, int tx, int ty)
{
	float offset = 0;
	//float offset = Game::TileSize / 2;
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
