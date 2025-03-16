#include "Engine/Math/Plane3.hpp"

#include "Engine/Math/MathUtils.hpp"


Plane3::Plane3(Vec3 const& normal, float distanceFromOriginAlongNormal)
	: m_normal(normal)
	, m_distanceFromOriginAlongNormal(distanceFromOriginAlongNormal)
{
}

Vec3 const Plane3::GetCenter() const
{
	return m_normal * m_distanceFromOriginAlongNormal;
}

bool Plane3::IsPointInFront(Vec3 const& referencePoint) const
{
	return GetProjectedLength3D(referencePoint, m_normal) > m_distanceFromOriginAlongNormal;
}

bool Plane3::IsPointBehind(Vec3 const& referencePoint) const
{
	return GetProjectedLength3D(referencePoint, m_normal) < m_distanceFromOriginAlongNormal;
}

bool Plane3::IsPointOn(Vec3 const& referencePoint) const
{
	return GetProjectedLength3D(referencePoint, m_normal) == m_distanceFromOriginAlongNormal;
}
