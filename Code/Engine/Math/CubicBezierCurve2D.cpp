#include "Engine/Math/CubicBezierCurve2D.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/CubicHermiteCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"

CubicBezierCurve2D::CubicBezierCurve2D(Vec2 const& startPos, Vec2 const& guidePos1, Vec2 const& guidePos2, Vec2 const& endPos)
	: m_startPos(startPos)
	, m_guidePos1(guidePos1)
	, m_guidePos2(guidePos2)
	, m_endPos(endPos)
{
}

CubicBezierCurve2D::CubicBezierCurve2D(CubicHermiteCurve2D const& fromHermite)
{
	m_startPos = fromHermite.m_startPos;
	m_endPos = fromHermite.m_endPos;
	m_guidePos1 = m_startPos + fromHermite.m_startVelocity * (1.f / 3.f);
	m_guidePos2 = m_endPos - fromHermite.m_endVelocity * (1.f / 3.f);
}

Vec2 const CubicBezierCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	if (parametricZeroToOne < 0.f)
	{
		return m_startPos;
	}

	if (parametricZeroToOne >= 1.f)
	{
		return m_endPos;
	}
	
	Vec2 lerpStartGuide1 = Interpolate(m_startPos, m_guidePos1, parametricZeroToOne);
	Vec2 lerpGuide1Guide2 = Interpolate(m_guidePos1, m_guidePos2, parametricZeroToOne);
	Vec2 lerpGuide2End = Interpolate(m_guidePos2, m_endPos, parametricZeroToOne);

	Vec2 lerpStartGuide1Guide2 = Interpolate(lerpStartGuide1, lerpGuide1Guide2, parametricZeroToOne);
	Vec2 lerpGuide1Guide2End = Interpolate(lerpGuide1Guide2, lerpGuide2End, parametricZeroToOne);

	return Interpolate(lerpStartGuide1Guide2, lerpGuide1Guide2End, parametricZeroToOne);
}

float CubicBezierCurve2D::GetApproximateLength(int numSubdivisions) const
{
	float approxLength = 0.f;

	float tStepPerSubdivision = 1.f / (float)numSubdivisions;
	float tEnd = 0.f;
	Vec2 segmentStart = m_startPos;

	for (int segmentIndex = 0; segmentIndex < numSubdivisions; segmentIndex++)
	{
		tEnd += tStepPerSubdivision;
		Vec2 segmentEnd = EvaluateAtParametric(tEnd);
		approxLength += GetDistance2D(segmentStart, segmentEnd);
		segmentStart = segmentEnd;
	}

	return approxLength;
}

Vec2 const CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	UNUSED(numSubdivisions);

	float remainingDistance = distanceAlongCurve;
	float tStepPerSubdivision = 1.f / (float)numSubdivisions;
	float tEnd = 0.f;
	Vec2 segmentStart = m_startPos;

	for (int segmentIndex = 0; segmentIndex < numSubdivisions; segmentIndex++)
	{
		tEnd += tStepPerSubdivision;
		Vec2 segmentEnd = EvaluateAtParametric(tEnd);
		float segmentLength = GetDistance2D(segmentStart, segmentEnd);
		if (segmentLength < remainingDistance)
		{
			remainingDistance -= segmentLength;
		}
		else
		{
			Vec2 segmentDirection = (segmentEnd - segmentStart).GetNormalized();
			return segmentStart + segmentDirection * remainingDistance;
		}
		segmentStart = segmentEnd;
	}

	return m_endPos;
}

void CubicBezierCurve2D::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, Rgba8 curveColor, Rgba8 pointsColor, bool showStartAndEndVelocities, Rgba8 velocityColor, int nnumSubivisions, float lineThickness, float pointRadius, float velocityScale, bool showGuidePositions) const
{
	float tStepPerSubdivision = 1.f / (float)nnumSubivisions;
	float t = 0.f;
	for (int segmentIndex = 0; segmentIndex < nnumSubivisions; segmentIndex++)
	{
		Vec2 startPoint = EvaluateAtParametric(t);
		t += tStepPerSubdivision;
		Vec2 endPoint = EvaluateAtParametric(t);
		AddVertsForLineSegment2D(verts, startPoint, endPoint, lineThickness, curveColor);
	}

	if (showStartAndEndVelocities)
	{
		CubicHermiteCurve2D hermiteCurve(*this);
		AddVertsForArrow2D(verts, hermiteCurve.m_startPos, hermiteCurve.m_startPos + hermiteCurve.m_startVelocity * velocityScale, lineThickness * 5.f, lineThickness, velocityColor);
		AddVertsForArrow2D(verts, hermiteCurve.m_endPos, hermiteCurve.m_endPos + hermiteCurve.m_endVelocity * velocityScale, lineThickness * 5.f, lineThickness, velocityColor);
	}

	AddVertsForDisc2D(verts, m_startPos, pointRadius, pointsColor);
	if (showGuidePositions)
	{
		AddVertsForDisc2D(verts, m_guidePos1, pointRadius, pointsColor);
		AddVertsForDisc2D(verts, m_guidePos2, pointRadius, pointsColor);
	}
	AddVertsForDisc2D(verts, m_endPos, pointRadius, pointsColor);
}
