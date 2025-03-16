#include "Engine/Math/OBB2.hpp"

#include "Engine/Math/MathUtils.hpp"

OBB2::OBB2(Vec2 const& center, Vec2 const& iBasisNormal, Vec2 const halfDimensions)
{
	m_center = center;
	m_iBasisNormal = iBasisNormal;
	m_halfDimensions = halfDimensions;
}

void OBB2::GetCornerPoints(Vec2* out_fourCornerWorldPositions) const
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();
	out_fourCornerWorldPositions[0] = m_center - (m_iBasisNormal * m_halfDimensions.x) - (jBasisNormal * m_halfDimensions.y);
	out_fourCornerWorldPositions[1] = m_center + (m_iBasisNormal * m_halfDimensions.x) - (jBasisNormal * m_halfDimensions.y);
	out_fourCornerWorldPositions[2] = m_center + (m_iBasisNormal * m_halfDimensions.x) + (jBasisNormal * m_halfDimensions.y);
	out_fourCornerWorldPositions[3] = m_center - (m_iBasisNormal * m_halfDimensions.x) + (jBasisNormal * m_halfDimensions.y);
}

Vec2 OBB2::GetLocalPosForWorldPos(Vec2 worldPos) const
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();

	Vec2 displacementCenterToPoint = worldPos - m_center;
	float localX = GetProjectedLength2D(displacementCenterToPoint, m_iBasisNormal);
	float localY = GetProjectedLength2D(displacementCenterToPoint, jBasisNormal);

	return Vec2(localX, localY);
}

Vec2 OBB2::GetWorldPosForLocalPos(Vec2 localPos) const
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();
	Vec2 worldPos = m_center + (m_iBasisNormal * localPos.x) + (jBasisNormal * localPos.y);

	return worldPos;
}

void OBB2::RotateAboutCenter(float rotationDeltaDegrees)
{
	m_iBasisNormal.RotateDegrees(rotationDeltaDegrees);
}
