#pragma once

#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

#include <map>


SpriteAnimPlaybackType GetPlaybackTypeFromString(std::string playbackTypeString);

class AnimationGroupDefinition
{
public:
	~AnimationGroupDefinition() = default;
	AnimationGroupDefinition() = default;
	AnimationGroupDefinition(XmlElement const* xmlElement, SpriteSheet* sheet);
	SpriteAnimDefinition GetAnimationForDirection(Vec3 const& direction) const;

public:
	std::string m_name;
	std::vector<Vec3> m_directions;
	std::vector<SpriteAnimDefinition> m_animations;
	bool m_scaleBySpeed = false;
};
