#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include <vector>

class CubicHermiteCurve2D;

class CubicBezierCurve2D
{
public:
	~CubicBezierCurve2D() = default;
	CubicBezierCurve2D() = default;
	CubicBezierCurve2D(CubicBezierCurve2D const& copyCurve) = default;
	explicit CubicBezierCurve2D(Vec2 const& startPos, Vec2 const& guidePos1, Vec2 const& guidePos2, Vec2 const& endPos);
	explicit CubicBezierCurve2D(CubicHermiteCurve2D const& fromHermite);
	Vec2 const EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec2 const EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
	void AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, Rgba8 curveColor, Rgba8 pointsColor, bool showStartAndEndVelocities = false, Rgba8 velocityColor = Rgba8::WHITE, int numSubdivisions = 64, float lineThickness = 0.2f, float pointRadius = 0.5f, float velocityScale = 1.f, bool showGuidePositions = true) const;

public:
	Vec2 m_startPos = Vec2::ZERO;
	Vec2 m_guidePos1 = Vec2::ZERO;
	Vec2 m_guidePos2 = Vec2::ZERO;
	Vec2 m_endPos = Vec2::ZERO;
};
