#pragma once

#include "Engine/Math/Vec2.hpp"

#include <vector>


struct ConvexPoly2
{
public:
	~ConvexPoly2() = default;
	ConvexPoly2() = default;
	ConvexPoly2(std::vector<Vec2> vertexesInPositiveThetaWindingOrder);

	void SetPositionForVertexAtIndex(Vec2 const& newPosition, int vertexIndex);

	std::vector<Vec2> const& GetVertexes() const;
	std::vector<Vec2>& GetVertexes();
	int GetVertexCount() const;

private:
	std::vector<Vec2> m_vertexes;
};
