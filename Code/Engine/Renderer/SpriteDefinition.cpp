#include "Engine/Renderer/SpriteDefinition.hpp"

#include "Engine/Renderer/Spritesheet.hpp"

SpriteDefinition::SpriteDefinition(SpriteSheet* spriteSheet, int spriteIndex, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs)
	: m_spriteSheet(spriteSheet)
	, m_spriteIndex(spriteIndex)
	, m_uvAtMins(uvAtMins)
	, m_uvAtMaxs(uvAtMaxs)
{
}

SpriteSheet* SpriteDefinition::GetSpriteSheet() const
{
	return m_spriteSheet;
}

Texture* SpriteDefinition::GetTexture() const
{
	return m_spriteSheet->GetTexture();
}