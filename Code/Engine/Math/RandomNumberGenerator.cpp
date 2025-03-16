#include "Engine/Math/RandomNumberGenerator.hpp"

#include <cstdlib>

int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	return (rand() % maxNotInclusive);
}

int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive)
{
	return (minInclusive + (rand() % (maxInclusive - minInclusive + 1)));
}

float RandomNumberGenerator::RollRandomFloatZeroToOne()
{
	return ((float)(rand()) / (float)(RAND_MAX));
}

float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive)
{
	return (float)(rand()) / (float)(RAND_MAX) * (maxInclusive - minInclusive) + minInclusive;
}

float RandomNumberGenerator::RollRandomFloatInRange(FloatRange range)
{
	return (float)(rand()) / (float)(RAND_MAX) * (range.m_max - range.m_min) + range.m_min;
}

Vec2 RandomNumberGenerator::RollRandomVec2InRange(float xMin, float xMax, float yMin, float yMax)
{
	return Vec2(RollRandomFloatInRange(xMin, xMax), RollRandomFloatInRange(yMin, yMax));
}

Vec2 RandomNumberGenerator::RollRandomVec2InBox(AABB2 const& box)
{
	return RollRandomVec2InRange(box.m_mins.x, box.m_maxs.x, box.m_mins.y, box.m_maxs.y);
}

Vec2 RandomNumberGenerator::RollRandomVec2InRadius(Vec2 const& center, float radius)
{
	return center + Vec2::MakeFromPolarDegrees(RollRandomFloatInRange(0, 360), RollRandomFloatInRange(0.f, radius));
}

bool RandomNumberGenerator::RollRandomChance(float chance)
{
	return RollRandomFloatZeroToOne() <= chance;
}

Vec3 RandomNumberGenerator::RollRandomVec3InAABB3(AABB3 box)
{
	return Vec3(RollRandomFloatInRange(box.m_mins.x, box.m_maxs.x), RollRandomFloatInRange(box.m_mins.y, box.m_maxs.y), RollRandomFloatInRange(box.m_mins.z, box.m_maxs.z));
}

Vec3 RandomNumberGenerator::RollRandomVec3InRadius(Vec3 const& center, float radius)
{
	return center + Vec3::MakeFromPolarDegrees(RollRandomFloatInRange(0.f, 360.f), RollRandomFloatInRange(-90.f, 90.f), radius);
}

