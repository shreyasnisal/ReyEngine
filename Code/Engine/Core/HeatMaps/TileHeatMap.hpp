#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/RaycastUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"

#include <vector>

/*! \brief Generic 2D heat map class
* 
* Heat maps are super helpful for various purposes such as map validation, storing distance fields, path-finding and more. This class only STORES heat maps and does not generate them.
* Create a heat map using the SetAllValues method and passing in the heat values. Once created, the heat map can then be used to find the heat value at any tile given its coordinates, debug drawing the heat map to visually indicate heat values, raycasting against the heat map, or generating a path from a source to a destination.
*/
class TileHeatMap
{
public:
	~TileHeatMap() = default; //! Default destructor
	TileHeatMap() = default; //! Default constructor
	TileHeatMap(TileHeatMap const& copyFrom) = default; //! Default copy constructor- constructs a heat map with the same data as the one passed in the argument
	explicit TileHeatMap(IntVec2 const& dimensions);

	void SetAllValues(std::vector<float> const& values);
	float GetValueAtTile(IntVec2 const& tileCoords) const;
	void SetValueAtTile(float value, IntVec2 const& tileCoords);

	void AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 bounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 const& specialColor) const;

	RaycastResult2D Raycast(Vec2 const& startPosition, Vec2 const& direction, float maxDistance) const;
	RaycastResult3D Raycast(Vec3 const& startPosition, Vec3 const& direction, float maxDistance, float minZ, float maxZ) const;

	std::vector<Vec2> GeneratePath(Vec2 const& source, Vec2 const& destination) const;

public:
	//! Dimensions of the heat map
	IntVec2 m_dimensions;
	//! Heat values stored in the heat map
	std::vector<float> m_values;
};