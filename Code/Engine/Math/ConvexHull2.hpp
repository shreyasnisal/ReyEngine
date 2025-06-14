#pragma once

#include "Engine/Math/Plane2.hpp"

#include <vector>


struct ConvexPoly2;


struct ConvexHull2
{
public:
	~ConvexHull2() = default;
	ConvexHull2() = default;
	ConvexHull2(ConvexHull2 const& copyFrom) = default;
	ConvexHull2(std::vector<Plane2> const& planes);
	ConvexHull2(ConvexPoly2 const& polyToCreateHullFrom);

	std::vector<Plane2> const& GetPlanes() const;
	int GetPlanesCount() const;

private:
	std::vector<Plane2> m_planes;
};
