#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"

class SpriteSheet;

struct SpriteDefinition
{
public:
	explicit SpriteDefinition(SpriteSheet* spriteSheet, int spriteIndex, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs);

	void					GetUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs) const { out_uvAtMins = m_uvAtMins; out_uvAtMaxs = m_uvAtMaxs; }
	AABB2					GetUVs() const { return AABB2(m_uvAtMins, m_uvAtMaxs); }
	SpriteSheet*			GetSpriteSheet() const;
	Texture*				GetTexture() const;
	float					GetAspect() const { return (m_uvAtMaxs.x - m_uvAtMins.x) / (m_uvAtMaxs.y - m_uvAtMins.y); }

public:
	int						m_spriteIndex = -1;

private:

private:
	SpriteSheet*			m_spriteSheet = nullptr;
	Vec2					m_uvAtMins = Vec2::ZERO;
	Vec2					m_uvAtMaxs = Vec2::ONE;
};