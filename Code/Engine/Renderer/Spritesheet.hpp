#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"

class SpriteSheet
{
public:
	explicit SpriteSheet(Texture* texture, IntVec2 const& simpleGridLayout);
	SpriteSheet(SpriteSheet const& copyFrom) = delete;

	Texture*						GetTexture() const { return m_texture; }
	int								GetNumSprites() const { return static_cast<int>(m_spriteDefs.size()); }
	SpriteDefinition const&			GetSpriteDef(int spriteIndex) const { return m_spriteDefs[spriteIndex]; }
	void							GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const { m_spriteDefs[spriteIndex].GetUVs(out_uvAtMins, out_uvAtMaxs); }
	AABB2							GetSpriteUVs(int spriteIndex) const { return m_spriteDefs[spriteIndex].GetUVs(); }
	float							GetAspect() const { return (float)m_texture->GetDimensions().x / (float)m_texture->GetDimensions().y; }

public:

private:

private:
	Texture*						m_texture = nullptr;
	std::vector<SpriteDefinition>	m_spriteDefs;

};