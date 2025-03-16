#include "Engine/Math/Splines.hpp"

#include "Engine/Math/CubicHermiteCurve2D.hpp"


CatmullRomSpline::CatmullRomSpline(std::vector<Vec2> positions)
	: m_positions(positions)
{
	m_velocities.resize(m_positions.size());

	m_velocities[0] = Vec2::ZERO;
	m_velocities.back() = Vec2::ZERO;

	for (int pointIndex = 1; pointIndex < (int)m_positions.size() - 1; pointIndex++)
	{
		m_velocities[pointIndex] = (m_positions[pointIndex + 1] - m_positions[pointIndex - 1]) * 0.5f;
	}
}

Vec2 const CatmullRomSpline::EvaluateAtParametric(float param) const
{
	int curveIndex = 0;
	float t = param;

	while (t > 1.f)
	{
		t -= 1.f;
		curveIndex++;
	}

	if (curveIndex >= (int)m_positions.size() - 1)
	{
		curveIndex = (int)m_positions.size() - 2;
		t = 1.f;
	}

	CubicHermiteCurve2D hermiteCurve = CubicHermiteCurve2D(m_positions[curveIndex], m_velocities[curveIndex], m_positions[curveIndex + 1], m_velocities[curveIndex + 1]);
	return hermiteCurve.EvaluateAtParametric(t);
}

float CatmullRomSpline::GetApproximateLength(int numSubdivisionsPerCurve) const
{
	float approxLength = 0.f;

	for (int curveIndex = 0; curveIndex < (int)m_positions.size() - 1; curveIndex++)
	{
		CubicHermiteCurve2D hermiteCurve(m_positions[curveIndex], m_velocities[curveIndex], m_positions[curveIndex + 1], m_velocities[curveIndex + 1]);
		approxLength += hermiteCurve.GetApproximateLength(numSubdivisionsPerCurve);
	}

	return approxLength;
}

Vec2 const CatmullRomSpline::EvaluateAtApproximateDistance(float distanceAlongSpline, int numSubdivisionsPerCruve) const
{
	float remainingDistance = distanceAlongSpline;

	for (int curveIndex = 0; curveIndex < (int)m_positions.size() - 1; curveIndex++)
	{
		CubicHermiteCurve2D hermiteCurve(m_positions[curveIndex], m_velocities[curveIndex], m_positions[curveIndex + 1], m_velocities[curveIndex + 1]);
		Vec2 pointAtDistance = hermiteCurve.EvaluateAtApproximateDistance(remainingDistance, numSubdivisionsPerCruve);
		if (pointAtDistance != hermiteCurve.m_endPos)
		{
			return pointAtDistance;
		}
		remainingDistance -= hermiteCurve.GetApproximateLength(numSubdivisionsPerCruve);
	}

	return m_positions.back();
}

void CatmullRomSpline::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, Rgba8 splineColor, Rgba8 pointsColor, bool showVelocities, Rgba8 velocityColor, int numSubdivisionsPerCurve, float lineThickness, float pointRadius, float velocityScale) const
{
	for (int curveIndex = 0; curveIndex < (int)m_positions.size() - 1; curveIndex++)
	{
		CubicHermiteCurve2D hermiteCurve(m_positions[curveIndex], m_velocities[curveIndex], m_positions[curveIndex + 1], m_velocities[curveIndex + 1]);
		hermiteCurve.AddVertsForDebugDraw(verts, splineColor, pointsColor, showVelocities, velocityColor, numSubdivisionsPerCurve, lineThickness, pointRadius, velocityScale, false);
	}
}
