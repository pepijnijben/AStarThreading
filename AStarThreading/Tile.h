#pragma once
#include "GameObject.h"

class Tile : public GameObject
{
private:
	Rect m_rect;
	Colour m_col;
public:
	Tile(float x, float y, float size, Colour color) : m_col(color)
	{
		m_rect.pos.x = x;
		m_rect.pos.y = y;
		m_rect.size.w = size;
		m_rect.size.h = size;
	};
	virtual ~Tile() {};

	void Render(Renderer& r) override
	{
		r.drawRect(m_rect, m_col);
	};
	void Update(unsigned int deltaTime) override 
	{
	};
};