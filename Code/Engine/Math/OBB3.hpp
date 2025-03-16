#pragma once

#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vec3.hpp"

struct OBB3
{
public:
	Vec3 m_center;
	Vec3 m_halfDimensions;
	Vec3 m_iBasis;
	Vec3 m_jBasis;
	Vec3 m_kBasis;

public:
	~OBB3() = default;
	OBB3() = default;
	OBB3(OBB3 const& copyFrom) = default;
	explicit OBB3(Vec3 const& center, Vec3 const& halfDimensions, Vec3 const& iBasis, Vec3 const& jBasis3D, Vec3 const& kBasis);
	explicit OBB3(Vec3 const& center, Vec3 const& halfDimensions, Vec3 const& iBasis, Vec3 const& jBasis3D);

	void GetCornerPoints(Vec3* out_fourCornerWorldPositions) const;
	Vec3 const GetLocalPosForWorldPos(Vec3 const& worldPos) const;
	Vec3 const GetWorldPosForLocalPos(Vec3 const& localPos) const;
	FloatRange const GetFloatRangeForPointsProjectedOntoAxis(Vec3 const& normalizedProjectionAxis) const;
};
