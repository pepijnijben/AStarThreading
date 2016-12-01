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
	DestroyAll();

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
				//AddEdge(current, current + addXVector);
			}
			if (emptyBelow)
			{
				//AddEdge(current, current + addYVector);
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

std::vector<Point2D> AStar::PathFromTo(Point2D from, Point2D to)
{
	ClearAll();

	if (!NodeExists(from) || !NodeExists(to))
	{
		throw std::exception("No such elements found");
	}

	if (from == to)
		return std::vector<Point2D>();

	std::priority_queue<AStarValue> pq;
	pq.push(AStarValue(nodeMap[from], 0, ManhattanDistance(from, to)));
	while (!pq.empty())
	{
		AStarValue aStarNode = pq.top();
		Node * node = aStarNode.node;
		pq.pop();

		AStarValue a1(node, 10, 10);
		AStarValue a2(node, 10, 10);

		if (a1 < a2)
		{
			
		}

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

	return std::vector<Point2D>();
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
