#include "Engine/Math/OBB3.hpp"

#include "Engine/Math/MathUtils.hpp"

OBB3::OBB3(Vec3 const& center, Vec3 const& halfDimensions, Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis)
	: m_center(center)
	, m_halfDimensions(halfDimensions)
	, m_iBasis(iBasis)
	, m_jBasis(jBasis)
	, m_kBasis(kBasis)
{
}

OBB3::OBB3(Vec3 const& center, Vec3 const& halfDimensions, Vec3 const& iBasis, Vec3 const& jBasis)
	: m_center(center)
	, m_halfDimensions(halfDimensions)
	, m_iBasis(iBasis)
	, m_jBasis(jBasis)
{
	m_kBasis = CrossProduct3D(m_iBasis, m_jBasis).GetNormalized();
}

void OBB3::GetCornerPoints(Vec3* out_fourCornerWorldPositions) const
{
	out_fourCornerWorldPositions[0] = m_center - m_iBasis * m_halfDimensions.x - m_jBasis * m_halfDimensions.y - m_kBasis * m_halfDimensions.z;
	out_fourCornerWorldPositions[1] = m_center - m_iBasis * m_halfDimensions.x - m_jBasis * m_halfDimensions.y + m_kBasis * m_halfDimensions.z;
	out_fourCornerWorldPositions[2] = m_center - m_iBasis * m_halfDimensions.x + m_jBasis * m_halfDimensions.y - m_kBasis * m_halfDimensions.z;
	out_fourCornerWorldPositions[3] = m_center - m_iBasis * m_halfDimensions.x + m_jBasis * m_halfDimensions.y + m_kBasis * m_halfDimensions.z;

	out_fourCornerWorldPositions[4] = m_center + m_iBasis * m_halfDimensions.x - m_jBasis * m_halfDimensions.y - m_kBasis * m_halfDimensions.z;
	out_fourCornerWorldPositions[5] = m_center + m_iBasis * m_halfDimensions.x - m_jBasis * m_halfDimensions.y + m_kBasis * m_halfDimensions.z;
	out_fourCornerWorldPositions[6] = m_center + m_iBasis * m_halfDimensions.x + m_jBasis * m_halfDimensions.y - m_kBasis * m_halfDimensions.z;
	out_fourCornerWorldPositions[7] = m_center + m_iBasis * m_halfDimensions.x + m_jBasis * m_halfDimensions.y + m_kBasis * m_halfDimensions.z;
}

Vec3 const OBB3::GetLocalPosForWorldPos(Vec3 const& worldPos) const
{
	Vec3 displacementCenterToPoint = worldPos - m_center;
	float localX = GetProjectedLength3D(displacementCenterToPoint, m_iBasis);
	float localY = GetProjectedLength3D(displacementCenterToPoint, m_jBasis);
	float localZ = GetProjectedLength3D(displacementCenterToPoint, m_kBasis);

	return Vec3(localX, localY, localZ);
}

Vec3 const OBB3::GetWorldPosForLocalPos(Vec3 const& localPos) const
{
	Vec3 worldPos = m_center + (m_iBasis * localPos.x) + (m_jBasis * localPos.y) + (m_kBasis * localPos.z);

	return worldPos;
}

FloatRange const OBB3::GetFloatRangeForPointsProjectedOntoAxis(Vec3 const& normalizedProjectionAxis) const
{
	FloatRange result;
	result.m_min = FLT_MAX;
	result.m_max = -FLT_MAX;

	Vec3 cornerPoints[8];
	GetCornerPoints(cornerPoints);
	for (int cornerIndex = 0; cornerIndex < 8; cornerIndex++)
	{
		float cornerProjLength = GetProjectedLength3D(cornerPoints[cornerIndex], normalizedProjectionAxis);
		if (cornerProjLength < result.m_min)
		{
			result.m_min = cornerProjLength;
		}
		if (cornerProjLength > result.m_max)
		{
			result.m_max = cornerProjLength;
		}
	}

	return result;
}
