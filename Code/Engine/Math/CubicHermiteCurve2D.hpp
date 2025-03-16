#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include <vector>

class CubicBezierCurve2D;

class CubicHermiteCurve2D
{
public:
	~CubicHermiteCurve2D() = default;
	CubicHermiteCurve2D() = default;
	CubicHermiteCurve2D(Vec2 const& startPosition, Vec2 const& startVelocity, Vec2 const& endPosition, Vec2 const& endVelocity);
	explicit CubicHermiteCurve2D(CubicBezierCurve2D const& fromBezier);
	Vec2 const EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec2 const EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
	void AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, Rgba8 curveColor, Rgba8 pointsColor, bool showStartAndEndVelocities = false, Rgba8 velocityColor = Rgba8::WHITE, int numSubdivisions = 64, float lineThickness = 0.2f, float pointRadius = 0.5f, float velocityScale = 1.f, bool showGuidePositions = false) const;

public:
	Vec2 m_startPos = Vec2::ZERO;
	Vec2 m_endPos = Vec2::ZERO;
	Vec2 m_startVelocity = Vec2::ZERO;
	Vec2 m_endVelocity = Vec2::ZERO;
};
