#pragma once

#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Spritesheet.hpp"

enum class SpriteAnimPlaybackType
{
	ONCE,
	LOOP,
	PINGPONG
};

class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, SpriteAnimPlaybackType playbackType=SpriteAnimPlaybackType::LOOP);
	SpriteAnimDefinition(SpriteSheet* spriteSheet);
	SpriteAnimDefinition() = default;

	const SpriteDefinition&						GetSpriteDefAtTime(float seconds) const;
	float										GetDuration() const { return m_durationSeconds; }
	SpriteAnimPlaybackType						GetPlaybackMode() const { return m_playbackType; }
	Texture*									GetTexture() const { return m_spriteSheet->GetTexture(); }
	void LoadFromXml(XmlElement const* element);

private:
	SpriteSheet*								m_spriteSheet = nullptr;
	int											m_startSpriteIndex = -1;
	int											m_endSpriteIndex = -1;
	float										m_durationSeconds = 1.f;
	SpriteAnimPlaybackType						m_playbackType = SpriteAnimPlaybackType::LOOP;
};