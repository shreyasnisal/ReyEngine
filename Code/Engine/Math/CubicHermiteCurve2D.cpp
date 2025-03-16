#include "Engine/Math/CubicHermiteCurve2D.hpp"

#include "Engine/Math/CubicBezierCurve2D.hpp"

CubicHermiteCurve2D::CubicHermiteCurve2D(Vec2 const& startPosition, Vec2 const& startVelocity, Vec2 const& endPosition, Vec2 const& endVelocity)
	: m_startPos(startPosition)
	, m_endPos(endPosition)
	, m_startVelocity(startVelocity)
	, m_endVelocity(endVelocity)
{
}

CubicHermiteCurve2D::CubicHermiteCurve2D(CubicBezierCurve2D const& fromBezier)
{
	m_startPos = fromBezier.m_startPos;
	m_endPos = fromBezier.m_endPos;
	m_startVelocity = 3.f * (fromBezier.m_guidePos1 - m_startPos);
	m_endVelocity = 3.f * (m_endPos - fromBezier.m_guidePos2);
}

Vec2 const CubicHermiteCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	CubicBezierCurve2D bezierCurve(*this);
	return bezierCurve.EvaluateAtParametric(parametricZeroToOne);
}

float CubicHermiteCurve2D::GetApproximateLength(int numSubdivisions) const
{
	CubicBezierCurve2D bezierCurve(*this);
	return bezierCurve.GetApproximateLength(numSubdivisions);
}

Vec2 const CubicHermiteCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	CubicBezierCurve2D bezierCurve(*this);
	return bezierCurve.EvaluateAtApproximateDistance(distanceAlongCurve, numSubdivisions);
}

void CubicHermiteCurve2D::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, Rgba8 curveColor, Rgba8 pointsColor, bool showStartAndEndVelocities, Rgba8 velocityColor, int nnumSubivisions, float lineThickness, float pointRadius, float velocityScale, bool showGuidePositions) const
{
	CubicBezierCurve2D bezierCurve(*this);
	bezierCurve.AddVertsForDebugDraw(verts, curveColor, pointsColor, showStartAndEndVelocities, velocityColor, nnumSubivisions, lineThickness, pointRadius, velocityScale, showGuidePositions);
}
