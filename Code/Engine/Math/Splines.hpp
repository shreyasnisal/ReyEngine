#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include <vector>

class CatmullRomSpline
{
public:
	~CatmullRomSpline() = default;
	CatmullRomSpline() = default;
	CatmullRomSpline(std::vector<Vec2> positions);
	Vec2 const EvaluateAtParametric(float param) const;
	float GetApproximateLength(int numSubdivisionsPerCurve = 64) const;
	Vec2 const EvaluateAtApproximateDistance(float distanceAlongSpline, int numSubdivisionsPerCurve = 64) const;
	void AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, Rgba8 splineColor, Rgba8 pointsColor, bool showVelocities = false, Rgba8 velocityColor = Rgba8::WHITE, int numSubdivisionsPerCurve = 64, float lineThickness = 0.2f, float pointRadius = 0.5f, float velocityScale = 1.f) const;

public:
	std::vector<Vec2> m_positions;
	std::vector<Vec2> m_velocities;
};
