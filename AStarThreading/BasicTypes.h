#pragma once
#include <vector>
#include <iostream>
#include <sstream>

class Edge;

class Point2D {
public:
	float x, y;
	Point2D(float _x = 0, float _y = 0) :x(_x), y(_y) { };
	float length() { return (float)sqrt(x*x + y*y); };

	std::string ToString()
	{
		std::stringstream ss;

		ss << "(" << x << ", " << y << ")";

		return ss.str();
	}

	friend Point2D operator+(const Point2D& a, const Point2D& b)
	{
		return Point2D(a.x + b.x, a.y + b.y);
	};
	friend Point2D operator-(const Point2D& a, const Point2D& b)
	{
		return Point2D(a.x - b.x, a.y - b.y);
	};


	bool operator==(const Point2D& a) const
	{
		return (a.x == x && a.y == y);
	}

	bool operator!=(const Point2D& a) const
	{
		return (a.x != x || a.y != y);
	}

	bool operator<(const Point2D& lhs) const
	{ 
		float l1 = (float)sqrt(x*x + y*y);
		float l2 = (float)sqrt(lhs.x*lhs.x + lhs.y*lhs.y);

		return l1 < l2;
	}
};


class Size2D {
public:
	float w, h;
	Size2D(float _w = 0, float  _h = 0) :w(_w), h(_h) {};
};

class Colour {
public:
	int r, g, b, a;
	Colour(int _r = 255, int  _g = 255, int _b = 255, int  _a = 255) :r(_r), g(_g), b(_b), a(_a) {};
};

class Rect {
public:
	Point2D pos;
	Size2D size;
	Rect(Point2D p, Size2D s) :pos(p), size(s) {};
	Rect(float x = 0, float y = 0, float w = 1, float h = 1) :pos(x, y), size(w, h) {};

};

enum NodeState { Untested, Closed, Open, Wall };

class Node
{
public:
	Point2D pos;
	std::vector<Edge> adjacent;
	float distance;
	Node * parent;
	NodeState state;

	Node(Point2D p) : pos(p)
	{
		Reset();
	}

	void Reset()
	{
		distance = std::numeric_limits<float>::infinity();
		parent = nullptr;
		state = NodeState::Untested;
	}
};

class Edge
{
public:
	Node * destination;
	float cost;

	Edge(Node * n, float c) : destination(n), cost(c) { }
};