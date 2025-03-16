#include "Engine/Renderer/SpriteAnimDefinition.hpp"

#include "Engine/Math/MathUtils.hpp"


SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType)
	: m_spriteSheet(sheet)
	, m_startSpriteIndex(startSpriteIndex)
	, m_endSpriteIndex(endSpriteIndex)
	, m_durationSeconds(durationSeconds)
	, m_playbackType(playbackType)
{
}

SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet* spriteSheet)
	: m_spriteSheet(spriteSheet)
{
}

void SpriteAnimDefinition::LoadFromXml(XmlElement const* element)
{
	m_startSpriteIndex = ParseXmlAttribute(*element, "startFrame", m_startSpriteIndex);
	m_endSpriteIndex = ParseXmlAttribute(*element, "endFrame", m_endSpriteIndex);
	m_durationSeconds *= (float)(m_endSpriteIndex - m_startSpriteIndex + 1);
}

SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{
	int spriteOffset = RoundDownToInt((seconds / m_durationSeconds) * static_cast<float>(m_endSpriteIndex - m_startSpriteIndex + 1));
	
	if (m_playbackType == SpriteAnimPlaybackType::ONCE)
	{
		spriteOffset = static_cast<int>(GetClamped(static_cast<float>(spriteOffset), 0.f, static_cast<float>(m_endSpriteIndex - m_startSpriteIndex)));
	}
	else if (m_playbackType == SpriteAnimPlaybackType::LOOP)
	{
		int numSprites = m_endSpriteIndex - m_startSpriteIndex + 1;
		spriteOffset = spriteOffset % numSprites;
	}
	else if (m_playbackType == SpriteAnimPlaybackType::PINGPONG)
	{
		int numSprites = (m_endSpriteIndex - m_startSpriteIndex + 1);
		spriteOffset = spriteOffset % (numSprites * 2 - 2);

		if (spriteOffset >= numSprites)
		{
			spriteOffset = (numSprites - 1) * 2 - spriteOffset;
		}
	}

	return m_spriteSheet->GetSpriteDef(m_startSpriteIndex + spriteOffset);
}

