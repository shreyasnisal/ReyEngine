#pragma once

#include "Engine/Math/Vec2.hpp"

/* \brief Axis-Aligned Bounding Box 2D
* 
* Represents an Axis-Aligned Bounding Box in 2D (AABB2), stored as two 2D vectors representing the bottom left and top right corners of the box.
* 
*/
struct AABB2
{
public:
	//! The minimum values in X and Y, represents the bottom left corner of the box
	Vec2 m_mins;
	//! The maximum values in X and Y, represents the top right corner of the box
	Vec2 m_maxs;

public:
	//! Default descructors for axis-aligned bounding boxes
	~AABB2() = default;
	//! Default constructor for axis-aligned bounding boxes
	AABB2() = default;
	//! Default copy constructor for axis-aligned bounding boxes
	AABB2(AABB2 const& copyFrom) = default;
	explicit AABB2(float minX, float minY, float maxX, float maxY);
	explicit AABB2(Vec2 const& mins, Vec2 const& maxs);

	bool		IsPointInside(Vec2 const& point) const;
	Vec2 const	GetCenter() const;
	Vec2 const	GetDimensions() const;
	Vec2 const	GetNearestPoint(Vec2 const& referencePoint) const;
	Vec2 const	GetPointAtUV(Vec2 const& uv) const;
	Vec2 const	GetUVForPoint(Vec2 const& point) const;
	AABB2 const	GetBoxAtUVs(Vec2 const& uvMins, Vec2 const& uvMaxs) const;

	void		Translate(Vec2 const& translationToApply);
	void		SetCenter(Vec2 const& newCenter);
	void		SetDimensions(Vec2 const& newDimensions);
	void		StretchToIncludePoint(Vec2 const& point);
	void		AddPadding(float xToAddOnBothSides, float yToAddOnBothSides);
	void		ReduceToAspect(float targetAspect);

public:
	//! Convenience name for AABB2(Vec2::ZERO, Vec2::ONE)
	static const AABB2 ZERO_TO_ONE;
};

const inline AABB2 AABB2::ZERO_TO_ONE = AABB2(Vec2::ZERO, Vec2::ONE);
