#include "AStar.h"
#include "Game.h"

std::vector<Point2D> AStar::PathFromTo(Point2D from, Point2D to)
{
	if ((from.x / Game::TileSize) >= Game::TileCount && (from.y / Game::TileSize) >= Game::TileCount
	&& (to.x / Game::TileSize) >= Game::TileCount && (to.y / Game::TileSize) >= Game::TileCount)
	{
		std::cout << "Could not find the from/to nodes, canceling A*" << std::endl;
		return std::vector<Point2D>();
	}

	vector<Tile> map = Game::gameObjects;
	vector<Point2D> waypoints = Game::m_waypoints;

	// Remove from point
	for (int i = 0; i < waypoints.size(); i++)
	{
		if (waypoints[i] == from)
		{
			waypoints.erase(waypoints.begin() + i);
			break;
		}
	}

	int fromId = ((from.x / Game::TileSize) * Game::TileCount) + (from.y / Game::TileSize);

	if (from == to)
		return std::vector<Point2D>();

	std::priority_queue<AStarValue> pq;
	pq.push(AStarValue(&map[fromId], 0, ManhattanDistance(from, to)));

	while (!pq.empty())
	{
		AStarValue aStarNode = pq.top();
		Tile * node = aStarNode.node;
		pq.pop();

		if (node->state == NodeState::Closed || node->state == NodeState::Wall)
		{
			continue;
		}
		node->state = NodeState::Closed;

		int currentId = ((node->m_rect.pos.x / Game::TileSize) * Game::TileCount) + (node->m_rect.pos.y / Game::TileSize);

		vector<Tile *> adjacent;

		if (currentId % Game::TileCount < Game::TileCount - 1)
			adjacent.push_back(&map[currentId + 1]);

		if (currentId % Game::TileCount > 0)
			adjacent.push_back(&map[currentId - 1]);

		if (currentId / Game::TileCount < Game::TileCount - 1)
			adjacent.push_back(&map[currentId + Game::TileCount]);

		if (currentId / Game::TileCount > 0)
			adjacent.push_back(&map[currentId - Game::TileCount]);

		for (auto adj : adjacent)
		{
			if (adj->m_rect.pos == to)
			{
				adj->parent = node;
				return BacktrackRoute(adj);
			}
			for (auto & wp : waypoints)
			{
				if (adj->m_rect.pos == wp)
				{
					adj->parent = node;
					return BacktrackRoute(adj);
				}
			}

			if (adj->state == NodeState::Closed || adj->state == NodeState::Wall)
			{
				continue;
			}

			float newDistance = aStarNode.Travelled + 20;

			if (adj->state == NodeState::Open) // Seen before but not closed yet
			{
				if (newDistance < adj->distance)
				{
					adj->parent = node;
					adj->distance = newDistance;
					pq.push(AStarValue(adj, newDistance, ManhattanDistance(adj->m_rect.pos, to)));
				}
			}
			else
			{
				adj->parent = node;
				adj->distance = newDistance;
				adj->state = NodeState::Open;
				pq.push(AStarValue(adj, newDistance, ManhattanDistance(adj->m_rect.pos, to)));
			}
		}
	}

	return std::vector<Point2D>();
}

std::vector<Point2D> AStar::PathFromTo(int fx, int fy, int tx, int ty)
{
	return PathFromTo(Point2D(fx * Game::TileSize, fy * Game::TileSize), Point2D(tx * Game::TileSize, ty * Game::TileSize));
}

std::vector<Point2D> AStar::BacktrackRoute(Tile * end)
{
	std::vector<Point2D> route;
	Tile * routeNode = end;
	while (routeNode != nullptr)
	{
		route.insert(route.begin(), routeNode->m_rect.pos);
		routeNode = routeNode->parent;
	}
	int x = 0;
	return route;
}
