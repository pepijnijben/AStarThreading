#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
private:
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
			if (tick > 2)
			{
				m_rect.pos = m_path[0];
				m_path.erase(m_path.begin());
				tick = 0;
			}
		}
	};

	void SetPath(std::vector<Point2D> path) { m_path = path; };
	Point2D GetPos() { return m_rect.pos; };
};