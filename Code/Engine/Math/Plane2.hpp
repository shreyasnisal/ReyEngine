#pragma once

#include "Engine/Math/Vec2.hpp"


struct Plane2
{
public:
	Vec2 m_normal = Vec2::NORTH;
	float m_distanceFromOriginAlongNormal = 0.f;

public:
	~Plane2() = default;
	Plane2() = default;
	Plane2(Plane2 const& copyFrom) = default;
	explicit Plane2(Vec2 const& normal, float distanceFromOriginAlongNormal);

	Vec2 const GetCenter() const;
	bool IsPointInFront(Vec2 const& referencePoint) const;
	bool IsPointBehind(Vec2 const& referencePoint) const;
	bool IsPointOn(Vec2 const& referencePoint) const;
};
