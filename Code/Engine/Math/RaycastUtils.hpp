#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Plane3.hpp"

struct RaycastResult2D
{
public:
	bool	m_didImpact			= false;
	float	m_impactDistance	= 0.f;
	Vec2	m_impactPosition;
	Vec2	m_impactNormal;

	Vec2	m_rayForwardNormal;
	Vec2	m_rayStartPosition;
	float	m_rayMaxLength		= 1.f;
};

struct RaycastResult3D
{
public:
	bool	m_didImpact = false;
	float	m_impactDistance = 0.f;
	Vec3	m_impactPosition;
	Vec3	m_impactNormal;

	Vec3	m_rayForwardNormal;
	Vec3	m_rayStartPosition;
	float	m_rayMaxLength = 1.f;
};

RaycastResult2D RaycastVsDisc2D(Vec2 const& startPos, Vec2 const& fwdNormal, float maxDistance, Vec2 const& discCenter, float discRadius);
RaycastResult2D RaycastVsLineSegment2D(Vec2 const& startPos, Vec2 const& fwdNormal, float maxDistance, Vec2 const& lineSegmentStart, Vec2 const& lineSegmentEnd);
RaycastResult2D	RaycastVsAABB2(Vec2 const& startPos, Vec2 const& fwdNormal, float maxDistance, AABB2 const& box);
RaycastResult3D RaycastVsCylinder3D(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius);
RaycastResult3D RaycastVsSphere(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, Vec3 const& sphereCenter, float sphereRadius);
RaycastResult3D RaycastVsAABB3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, AABB3 const& box);
RaycastResult3D RaycastVsOBB3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, OBB3 const& orientedBox);
RaycastResult3D RaycastVsPlane3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, Plane3 const& plane);
