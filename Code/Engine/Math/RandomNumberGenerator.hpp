#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/FloatRange.hpp"

class RandomNumberGenerator
{
public:
	int			RollRandomIntLessThan(int maxNotInclusive);
	int			RollRandomIntInRange(int minInclusive, int maxInclusive);
	float		RollRandomFloatZeroToOne();
	float		RollRandomFloatInRange(float minInclusive, float maxInclusive);
	float		RollRandomFloatInRange(FloatRange range);
	Vec2		RollRandomVec2InRange(float xMin, float xMax, float yMin, float yMax);
	Vec2		RollRandomVec2InBox(AABB2 const& box);
	Vec2		RollRandomVec2InRadius(Vec2 const& center, float radius);
	bool		RollRandomChance(float chance);
	Vec3		RollRandomVec3InAABB3(AABB3 box);
	Vec3		RollRandomVec3InRadius(Vec3 const& center, float radius);
};
