#include "Engine/Core/HeatMaps/TileHeatMap.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

/*! \brief Creates a heat map filled with zeros
* 
* Resizes the heat map values vector to the dimensions.x \* dimensions.y and fills the vector with all zeros.
* \param Dimensions An IntVec2 containing the target dimensions of the heat map
* 
*/
TileHeatMap::TileHeatMap(IntVec2 const& dimensions)
	: m_dimensions(dimensions)
{
	m_values.resize(dimensions.x * dimensions.y);
	fill(m_values.begin(), m_values.end(), 0.f);
}

/*!
* 
* \brief Sets all values with the values in the given list
* 
* \param values A list of float values indicating the heat value for each tile
* 
*/
void TileHeatMap::SetAllValues(std::vector<float> const& values)
{
	m_values = values;
}

/*!
* \brief Sets heat value for a specific tile
*
* Does nothing if the coordinates of the tile are outside the bounds of the heat map
* Computes the tile index based on the given tile coordinates and dimensions of the heat map and sets the heat value at that index to the provided value
* \param value: A float indicating the heat value to set for the tile
* \param tileCoords: An IntVec2 containing the coordinates of the tile
*
*/
void TileHeatMap::SetValueAtTile(float value, IntVec2 const& tileCoords)
{
	if (tileCoords.x < 0 || tileCoords.x > m_dimensions.x - 1 || tileCoords.y < 0 || tileCoords.y > m_dimensions.y - 1)
	{
		return;
	}

	int tileIndex = tileCoords.x + tileCoords.y * m_dimensions.x;
	m_values[tileIndex] = value;
}

/*! \brief Gets heat value for a specific tile
* 
* Fatal error if the tile coordinates passed in the argument are out of bounds for the heat map.
* Computes the tile index based on the given tile coordinates and dimensions of the heat map and returns the heat value at that index.
* \param tileCoords An IntVec2 containing the coordinates of the tile
* \return The heat value at those tile coordinates
* 
*/
float TileHeatMap::GetValueAtTile(IntVec2 const& tileCoords) const
{
	if (tileCoords.x < 0 || tileCoords.x > m_dimensions.x - 1 || tileCoords.y < 0 || tileCoords.y > m_dimensions.y - 1)
	{
		ERROR_AND_DIE("TileHeatMap GetValue called on invalid tile coordinates!");
	}

	int tileIndex = tileCoords.x + tileCoords.y * m_dimensions.x;
	return m_values[tileIndex];
}

/*! \brief Adds vertexes for debug drawing the heat map
* 
* Adds vertexes to the verts argument passed in (by reference) to debug draw the heat map in the bounds passed in. Maps the minimum value in the heat values to the lowColor and the maximum value (excluding the special value) to the highColor. Colors for tiles with heat values between the minimum and maximum are drawn in a color obtained by interpolating the heat value from the minimum value to the maximum value. Adds vertexes for tiles with specialValue using the specialColor.
* \param verts A list of Vertex_PCU to which the new vertexes are added (passed by reference)
* \param bounds An AABB2 that dictates where the vertexes should be added
* \param valueRange A FloatRange used for mapping the lowColor and highColor
* \param lowColor The color to be used as the minimum for interpolating tile colors
* \param highColor The color to be used as the maximum for interpolating tile colors
* \param specialValue A float indicating which value to use as the special value for tiles, with tiles having value equal to the special value being added using the specialColor
* \param specialColor The color to be used for tiles with the special value
* 
*/
void TileHeatMap::AddVertsForDebugDraw(std::vector<Vertex_PCU>& verts, AABB2 bounds, FloatRange valueRange, Rgba8 lowColor, Rgba8 highColor, float specialValue, Rgba8 const& specialColor) const
{
	float boundsWidth = bounds.GetDimensions().x;
	float boundsHeight = bounds.GetDimensions().y;

	for (int tileIndex = 0; tileIndex < m_dimensions.x * m_dimensions.y; tileIndex++)
	{
		float tileMinsX = static_cast<float>(tileIndex % static_cast<int>(boundsWidth));
		float tileMinsY = static_cast<float>(tileIndex / static_cast<int>(boundsWidth));
		float tileMaxsX = tileMinsX + boundsWidth / static_cast<float>(m_dimensions.x);
		float tileMaxsY = tileMinsY + boundsHeight / static_cast<float>(m_dimensions.y);

		Rgba8 tileColor = specialColor;
		if (m_values[tileIndex] != specialValue)
		{
			float tileValue = RangeMapClamped(m_values[tileIndex], valueRange.m_min, valueRange.m_max, 0.f, 1.f);
			tileColor = Interpolate(lowColor, highColor, tileValue);
		}

		AddVertsForAABB2(verts, AABB2(Vec2(tileMinsX, tileMinsY), Vec2(tileMaxsX, tileMaxsY)), tileColor);
	}
}

/*! \brief Performs a raycast against the heat map
* 
* Uses the Amanatides-Woo algorithm for a fast voxel raycast against the heat map. Usually useful for solid maps (maps with binary values for the heat).
* \param startPosition A Vec2 indicating the start position of the ray
* \param direction A Vec2 indicating the direction of the ray
* \param maxDistance A float indicating the max distance along the ray for which impacts should be reported
* \return A RaycastResult2D with the result info
* \sa RaycastResult2D
* 
*/
RaycastResult2D TileHeatMap::Raycast(Vec2 const& startPosition, Vec2 const& direction, float maxDistance) const
{
	RaycastResult2D raycastResult;

	if (maxDistance == 0)
	{
		return raycastResult;
	}

	int dimensionX = m_dimensions.x;
	int dimensionY = m_dimensions.y;

	IntVec2 currentTile = IntVec2(RoundDownToInt(startPosition.x), RoundDownToInt(startPosition.y));
	Vec2 rayStepSize = Vec2(direction.x != 0 ? 1.f / fabsf(direction.x) : 99999.f, direction.y != 0 ? 1.f / fabsf(direction.y) : 99999.f);
	Vec2 cumulativeRayLengthIn1D;
	IntVec2 directionXY;
	float totalRayLength = 0.f;

	float deltaRayLength = 0.f;

	if (direction.x < 0.f)
	{
		directionXY.x = -1;
		cumulativeRayLengthIn1D.x = (startPosition.x - static_cast<float>(currentTile.x)) * rayStepSize.x;
	}
	else
	{
		directionXY.x = 1;
		cumulativeRayLengthIn1D.x = (static_cast<float>(currentTile.x) + 1.f - startPosition.x) * rayStepSize.x;
	}

	if (direction.y < 0)
	{
		directionXY.y = -1;
		cumulativeRayLengthIn1D.y = (startPosition.y - static_cast<float>(currentTile.y)) * rayStepSize.y;
	}
	else
	{
		directionXY.y = 1;
		cumulativeRayLengthIn1D.y = (static_cast<float>(currentTile.y) + 1.f - startPosition.y) * rayStepSize.y;
	}

	while (totalRayLength < maxDistance)
	{
		if (currentTile.x < 0 || currentTile.y < 0 || currentTile.x > dimensionX - 1 || currentTile.y > dimensionY)
		{
			return raycastResult;
		}

		if (GetValueAtTile(currentTile) != 1.f)
		{
			Vec2 hitPoint = startPosition + direction * (totalRayLength - deltaRayLength);
			raycastResult.m_didImpact = true;
			raycastResult.m_impactDistance = totalRayLength - deltaRayLength;
			raycastResult.m_impactPosition = hitPoint;
			return raycastResult;
		}

		if (cumulativeRayLengthIn1D.x < cumulativeRayLengthIn1D.y)
		{
			currentTile.x += directionXY.x;
			totalRayLength = cumulativeRayLengthIn1D.x;
			cumulativeRayLengthIn1D.x += rayStepSize.x;
			raycastResult.m_impactNormal = -Vec2::EAST * (float)directionXY.x;
		}
		else
		{
			currentTile.y += directionXY.y;
			totalRayLength = cumulativeRayLengthIn1D.y;
			cumulativeRayLengthIn1D.y += rayStepSize.y;
			raycastResult.m_impactNormal = -Vec2::NORTH * (float)directionXY.y;
		}
	}

	raycastResult.m_didImpact = false;
	raycastResult.m_impactNormal = Vec2::ZERO;
	raycastResult.m_impactPosition = startPosition + direction * maxDistance;
	raycastResult.m_impactDistance = maxDistance;
	return raycastResult;
}

/*! \brief Performs a raycast against the heat map for Z values between the minimum and maximum provided
*
* Performs a 2D raycast against the heat map but only reports impacts within the minZ and maxZ provided. If an impact outside of this range is detected, the impact is ignored and the raycast is continued.
* \param startPosition A Vec3 indicating the start position of the ray
* \param direction A Vec3 indicating the direction of the ray
* \param maxDistance A float indicating the max distance along the ray for which impacts should be reported
* \param minZ A float indicating the minimum Z value for which an impact should be reported
* \param maxAZ A float indicating the maximum Z value for which an impact should be reported
* \return A RaycastResult3D with the result info
* \sa RaycastResult3D
*
*/
RaycastResult3D TileHeatMap::Raycast(Vec3 const& startPosition, Vec3 const& direction, float maxDistance, float minZ, float maxZ) const
{
	RaycastResult3D raycastResult;

	if (maxDistance == 0)
	{
		return raycastResult;
	}

	int dimensionX = m_dimensions.x;
	int dimensionY = m_dimensions.y;

	IntVec2 currentTile = IntVec2(RoundDownToInt(startPosition.x), RoundDownToInt(startPosition.y));
	Vec2 rayStepSize = Vec2(direction.x != 0 ? 1.f / fabsf(direction.x) : 99999.f, direction.y != 0 ? 1.f / fabsf(direction.y) : 99999.f);
	Vec2 cumulativeRayLengthIn1D;
	IntVec2 directionXY;
	float totalRayLength = 0.f;

	float deltaRayLength = 0.f;

	if (direction.x < 0.f)
	{
		directionXY.x = -1;
		cumulativeRayLengthIn1D.x = (startPosition.x - static_cast<float>(currentTile.x)) * rayStepSize.x;
	}
	else
	{
		directionXY.x = 1;
		cumulativeRayLengthIn1D.x = (static_cast<float>(currentTile.x) + 1.f - startPosition.x) * rayStepSize.x;
	}

	if (direction.y < 0)
	{
		directionXY.y = -1;
		cumulativeRayLengthIn1D.y = (startPosition.y - static_cast<float>(currentTile.y)) * rayStepSize.y;
	}
	else
	{
		directionXY.y = 1;
		cumulativeRayLengthIn1D.y = (static_cast<float>(currentTile.y) + 1.f - startPosition.y) * rayStepSize.y;
	}

	while (totalRayLength < maxDistance)
	{
		if (currentTile.x < 0 || currentTile.y < 0 || currentTile.x > dimensionX - 1 || currentTile.y > dimensionY - 1)
		{
			return raycastResult;
		}

		if (GetValueAtTile(currentTile) != 1.f)
		{
			Vec2 impactPosition2D = startPosition.GetXY() + direction.GetXY() * (totalRayLength - deltaRayLength);
			//float impactDistance2D = totalRayLength - deltaRayLength;

			float impactDistance = GetDistance2D(impactPosition2D, startPosition.GetXY());
			if (!AreFloatsMostlyEqual(direction.x, 0.f, 0.0001f))
			{
				impactDistance = (impactPosition2D.x - startPosition.x) / direction.x;
			}
			float raycastVsTilesImpactZ = startPosition.z + direction.z * impactDistance;
			if (raycastVsTilesImpactZ >= minZ && raycastVsTilesImpactZ <= maxZ && impactDistance < maxDistance)
			{
				raycastResult.m_didImpact = true;
				raycastResult.m_impactPosition = startPosition + impactDistance * direction;
				raycastResult.m_impactDistance = impactDistance;
				return raycastResult;
			}
		}

		if (cumulativeRayLengthIn1D.x < cumulativeRayLengthIn1D.y)
		{
			currentTile.x += directionXY.x;
			totalRayLength = cumulativeRayLengthIn1D.x;
			cumulativeRayLengthIn1D.x += rayStepSize.x;
			raycastResult.m_impactNormal = -Vec3::EAST * (float)directionXY.x;
		}
		else
		{
			currentTile.y += directionXY.y;
			totalRayLength = cumulativeRayLengthIn1D.y;
			cumulativeRayLengthIn1D.y += rayStepSize.y;
			raycastResult.m_impactNormal = -Vec3::NORTH * (float)directionXY.y;
		}
	}

	raycastResult.m_didImpact = false;
	raycastResult.m_impactNormal = Vec3::ZERO;
	raycastResult.m_impactPosition = startPosition + direction * maxDistance;
	raycastResult.m_impactDistance = maxDistance;
	return raycastResult;
}

/*! \brief Generates a path from a given source to a destination, following (one of) the fastest path
* 
* Follows the fastest path "downhill" (decreasing heat values) from a source tile to a destination tile. Usually used after generating a heat map with the destination heat value set to zero.
* If a conflict between the lowest heat value is found at any point (two possible paths are equally fast), this method prioritizes the south, north and west tiles in that order.
* \param source An IntVec2 indicating the coordinates of the source tile
* \param destination An IntVec2 indicating the coordinates of the destination tile
* \return A list of Vec2 with points along the shortest path with the destination as the first value
* 
*/
std::vector<Vec2> TileHeatMap::GeneratePath(Vec2 const& source, Vec2 const& destination) const
{
	std::vector<Vec2> path;
	
	IntVec2 sourceTile = IntVec2(RoundDownToInt(source.x), RoundDownToInt(source.y));
	IntVec2 destinationTile = IntVec2(RoundDownToInt(destination.x), RoundDownToInt(destination.y));

	IntVec2 currentTile = sourceTile;
	float minHeatValue = GetValueAtTile(sourceTile);
	while (currentTile != destinationTile)
	{
		IntVec2 southTile = currentTile + IntVec2::SOUTH;
		IntVec2 northTile = currentTile + IntVec2::NORTH;
		IntVec2 westTile = currentTile + IntVec2::WEST;
		IntVec2 eastTile = currentTile + IntVec2::EAST;

		if (southTile.y >= 0 && GetValueAtTile(southTile) < minHeatValue)
		{
			minHeatValue = GetValueAtTile(southTile);
			currentTile = southTile;
		}

		if (northTile.y < m_dimensions.y && GetValueAtTile(northTile) < minHeatValue)
		{
			minHeatValue = GetValueAtTile(northTile);
			currentTile = northTile;
		}

		if (westTile.x >= 0 && GetValueAtTile(westTile) < minHeatValue)
		{
			minHeatValue = GetValueAtTile(westTile);
			currentTile = westTile;
		}

		if (eastTile.x < m_dimensions.x && GetValueAtTile(eastTile) < minHeatValue)
		{
			minHeatValue = GetValueAtTile(eastTile);
			currentTile =  eastTile;
		}

		path.push_back(Vec2(static_cast<float>(currentTile.x) + 0.5f, static_cast<float>(currentTile.y) + 0.5f));
	}

	path.push_back(destination);

	std::reverse(std::begin(path), std::end(path));
	return path;
}
