#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
private:
	bool m_findingPath;
	Rect m_rect;
	std::vector<Point2D> m_path;
	uint8_t tick = 0;
	Colour m_col = Colour(150, 150, 0);
public:
	Enemy(float x, float y, float size)
	{
		m_rect.pos.x = x;
		m_rect.pos.y = y;
		m_rect.size.w = size;
		m_rect.size.h = size;
		m_findingPath = false;
	};
	virtual ~Enemy() {};

	void Render(Renderer& r) override
	{
		r.drawRect(m_rect, m_col);
	};
	void Update(unsigned int deltaTime) override
	{
		if (m_path.size() > 0)
		{
			tick++;
			if (tick >= 1)
			{
				m_rect.pos = m_path[0];
				m_path.erase(m_path.begin());
				tick = 0;
			}
		}
	};

	void SetPath(std::vector<Point2D> path) { m_path = path; m_findingPath = false; };
	bool PathDone() { return m_path.size() > 0 ? false : true; };
	bool FindingPath() { return m_findingPath; };
	void FindingPath(bool val) { m_findingPath = val; };
	Point2D GetPos() { return m_rect.pos; };
};