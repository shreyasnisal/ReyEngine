#include "Engine/Renderer/AnimationGroupDefinition.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/MathUtils.hpp"

SpriteAnimPlaybackType GetPlaybackTypeFromString(std::string playbackTypeStr)
{
	if (!strcmp("Once", playbackTypeStr.c_str()))
	{
		return SpriteAnimPlaybackType::ONCE;
	}
	if (!strcmp("PingPong", playbackTypeStr.c_str()))
	{
		return SpriteAnimPlaybackType::PINGPONG;
	}

	return SpriteAnimPlaybackType::LOOP;
}

AnimationGroupDefinition::AnimationGroupDefinition(XmlElement const* xmlElement, SpriteSheet* sheet)
{
	m_name = ParseXmlAttribute(*xmlElement, "name", m_name);
	float animationSecondsPerFrame = ParseXmlAttribute(*xmlElement, "secondsPerFrame", 0.f);
	std::string playbackTypeStr = ParseXmlAttribute(*xmlElement, "playbackMode", "Loop");
	SpriteAnimPlaybackType animationsPlaybackType = GetPlaybackTypeFromString(playbackTypeStr);
	m_scaleBySpeed = ParseXmlAttribute(*xmlElement, "scaleBySpeed", m_scaleBySpeed);

	XmlElement const* directionElement = xmlElement->FirstChildElement("Direction");
	while (directionElement)
	{
		Vec3 direction = ParseXmlAttribute(*directionElement, "vector", Vec3::ZERO).GetNormalized();
		XmlElement const* animationElement = directionElement->FirstChildElement("Animation");
		if (!animationElement)
		{
			g_console->AddLine(DevConsole::WARNING, "No animation element was found in the direction element of an animation group, the animation will be ignored");
			directionElement = directionElement->NextSiblingElement();
			continue;
		}

		m_directions.push_back(direction);
		SpriteAnimDefinition animation = SpriteAnimDefinition(sheet, -1, -1, animationSecondsPerFrame, animationsPlaybackType);
		animation.LoadFromXml(animationElement);
		m_animations.push_back(animation);

		if (animationElement->NextSiblingElement() != nullptr)
		{
			g_console->AddLine(DevConsole::WARNING, "Multiple animations found for the same direction; only the first animation will be used!");
		}

		directionElement = directionElement->NextSiblingElement();
	}
}

SpriteAnimDefinition AnimationGroupDefinition::GetAnimationForDirection(Vec3 const& direction) const
{
	float largestDotProduct = FLT_MIN;
	int resultAnimationIndex = 0;

	for (int animationIndex = 0; animationIndex < (int)m_animations.size(); animationIndex++)
	{
		float dotProduct = DotProduct3D(direction, m_directions[animationIndex]);
		if (dotProduct > largestDotProduct)
		{
			resultAnimationIndex = animationIndex;
			largestDotProduct = dotProduct;
		}
	}

	return m_animations[resultAnimationIndex];
}
