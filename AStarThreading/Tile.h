#pragma once
#include "GameObject.h"

class Tile : public GameObject
{
private:
	short m_spriteId;
public:
	Rect m_rect;
	float distance;
	uint8_t state;
	Tile * parent;

	Tile(float x, float y, float size, short si = 0, uint8_t s = 0) : state(s), parent(nullptr), m_spriteId(si)
	{
		m_rect.pos.x = x;
		m_rect.pos.y = y;
		m_rect.size.w = size;
		m_rect.size.h = size;
	};
	virtual ~Tile() {};

	void Render(Renderer& r) override
	{
		r.drawSprite(m_rect, m_spriteId);
	};
	void Update(unsigned int deltaTime) override 
	{
	};
};