#include "Engine/Math/Plane2.hpp"

#include "Engine/Math/MathUtils.hpp"


Plane2::Plane2(Vec2 const& normal, float distanceFromOriginAlongNormal)
	: m_normal(normal)
	, m_distanceFromOriginAlongNormal(distanceFromOriginAlongNormal)
{
}

Vec2 const Plane2::GetCenter() const
{
	return m_normal * m_distanceFromOriginAlongNormal;
}

bool Plane2::IsPointInFront(Vec2 const& referencePoint) const
{
	return GetProjectedLength2D(referencePoint, m_normal) > m_distanceFromOriginAlongNormal;
}

bool Plane2::IsPointBehind(Vec2 const& referencePoint) const
{
	return GetProjectedLength2D(referencePoint, m_normal) > m_distanceFromOriginAlongNormal;
}

bool Plane2::IsPointOn(Vec2 const& referencePoint) const
{
	return GetProjectedLength2D(referencePoint, m_normal) == m_distanceFromOriginAlongNormal;
}
