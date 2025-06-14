#include "Engine/Math/ConvexHull2.hpp"

#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Math/MathUtils.hpp"


ConvexHull2::ConvexHull2(std::vector<Plane2> const& planes)
	: m_planes(planes)
{
}

ConvexHull2::ConvexHull2(ConvexPoly2 const& polyToCreateHullFrom)
{
	std::vector<Vec2> vertexes = polyToCreateHullFrom.GetVertexes();

	for (int vertexIndex = 0; vertexIndex < (int)vertexes.size() - 1; vertexIndex++)
	{
		Plane2 plane;
		Vec2 sideDirection = (vertexes[vertexIndex + 1] - vertexes[vertexIndex]).GetNormalized();
		plane.m_normal = sideDirection.GetRotatedMinus90Degrees();
		plane.m_distanceFromOriginAlongNormal = DotProduct2D(vertexes[vertexIndex], plane.m_normal);
		m_planes.push_back(plane);
	}

	// Handle last plane manually
	Plane2 plane;
	Vec2 sideDirection = (vertexes[0] - vertexes[vertexes.size() - 1]).GetNormalized();
	plane.m_normal = sideDirection.GetRotatedMinus90Degrees();
	plane.m_distanceFromOriginAlongNormal = DotProduct2D(vertexes[0], plane.m_normal);
	m_planes.push_back(plane);
}

std::vector<Plane2> const& ConvexHull2::GetPlanes() const
{
	return m_planes;
}

int ConvexHull2::GetPlanesCount() const
{
	return (int)m_planes.size();;
}
