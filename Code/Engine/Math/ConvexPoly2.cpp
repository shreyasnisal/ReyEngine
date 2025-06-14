#include "Engine/Math/ConvexPoly2.hpp"


ConvexPoly2::ConvexPoly2(std::vector<Vec2> vertexesInPositiveThetaWindingOrder)
	: m_vertexes(vertexesInPositiveThetaWindingOrder)
{
}

void ConvexPoly2::SetPositionForVertexAtIndex(Vec2 const& newPosition, int vertexIndex)
{
	if (vertexIndex < 0 || vertexIndex >= m_vertexes.size())
	{
		return;
	}

	m_vertexes[vertexIndex] = newPosition;
}

std::vector<Vec2> const& ConvexPoly2::GetVertexes() const
{
	return m_vertexes;
}

std::vector<Vec2>& ConvexPoly2::GetVertexes()
{
	return m_vertexes;
}

int ConvexPoly2::GetVertexCount() const
{
	return (int)m_vertexes.size();
}
