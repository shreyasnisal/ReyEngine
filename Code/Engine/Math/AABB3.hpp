#pragma once

#include "Engine/Math/Vec3.hpp"

/*! Axis-Aligned Bounding Box 3D
* 
* Represents an Axis-Aligned Bounding Box in 3D (AABB3), stored as two 3D vectors representing the bottom right near corner and top left far corners of the box.
* 
*/
struct AABB3
{
public:
	//! The bottom right near corner of the box
	Vec3 m_mins;
	//! The top left far corner of the box
	Vec3 m_maxs;

public:
	//! Default destructor for AABB3 objects
	~AABB3() = default;
	//! Default constructor for AABB3 objects
	AABB3() = default;
	//! Default copy constructor for AABB3 objects
	AABB3(AABB3 const& copyFrom) = default;
	explicit AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	explicit AABB3(Vec3 const& mins, Vec3 const& maxs);

	void					SetFromText(char const* text);

	bool					IsPointInside(Vec3 const& point) const;
	Vec3 const				GetCenter() const;
	Vec3 const				GetDimensions() const;
	Vec3 const				GetNearestPoint(Vec3 const& referencePoint) const;
	
	void					Translate(Vec3 const& translationToApply);
	void					SetCenter(Vec3 const& newCenter);
	void					SetDimensions(Vec3 const& newDimensions);
	void					StretchToIncludePoint(Vec3 const& point);
	void					GetCornerPoints(Vec3* out_cornerPoints) const;
};
