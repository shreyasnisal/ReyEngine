#pragma once

#include "Engine/Math/Vec3.hpp"


struct Plane3
{
public:
	Vec3 m_normal = Vec3::SKYWARD;
	float m_distanceFromOriginAlongNormal = 0.f;

public:
	~Plane3() = default;
	Plane3() = default;
	Plane3(Plane3 const& copyFrom) = default;
	explicit Plane3(Vec3 const& normal, float distanceFromOriginAlongNormal);

	Vec3 const GetCenter() const;
	bool IsPointInFront(Vec3 const& referencePoint) const;
	bool IsPointBehind(Vec3 const& referencePoint) const;
	bool IsPointOn(Vec3 const& referencePoint) const;
};
