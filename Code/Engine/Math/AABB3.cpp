#include "Engine/Math/AABB3.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"


/*! \brief Constructs an AABB3 from the Vec3 coordinates of the bottom right near corner and top left far corner
* 
* \param mins A Vec3 representing the coordinates of the bottom right near corner of the box
* \param maxs A Vec3 representing the coordinates of the top left far corner of the box
* 
*/
AABB3::AABB3(Vec3 const& mins, Vec3 const& maxs)
	: m_mins(mins)
	, m_maxs(maxs)
{
}

void AABB3::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, ',');

	if (numSplitStrings != 6)
	{
		ERROR_AND_DIE("Incorrect number of literals in AABB3 string!");
	}

	m_mins.x = static_cast<float>(atof(splitStrings[0].c_str()));
	m_mins.y = static_cast<float>(atof(splitStrings[1].c_str()));
	m_mins.z = static_cast<float>(atof(splitStrings[2].c_str()));
	m_maxs.x = static_cast<float>(atof(splitStrings[3].c_str()));
	m_maxs.y = static_cast<float>(atof(splitStrings[4].c_str()));
	m_maxs.z = static_cast<float>(atof(splitStrings[5].c_str()));
}

/*! \brief Constructs an AABB3 from six floats representing the minimum and maximum XYZ values
* 
* \param minX The minimum X value (for the bottom right near corner)
* \param minY The minimum Y value (for the bottom right near corner)
* \param minZ The minimum Z value (for the bottom right near corner)
* \param maxX The maximum X value (for the top left far corner)
* \param maxY The maximum Y value (for the top left far corner)
* \param maxZ The maximum Z value (for the top left far corner)
* 
*/
AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	: m_mins(minX, minY, minZ)
	, m_maxs(maxX, maxY, maxZ)
{
}

/*! \brief Checks whether a point is inside the AABB3 or not
* 
* \param point A Vec3 representing the point for which the mehod should check if it is inside or not
* \return A boolean indicating whether the point is inside
* 
*/
bool AABB3::IsPointInside(Vec3 const& point) const
{
	return (
		(point.x > m_mins.x && point.x < m_maxs.x) &&
		(point.y > m_mins.y && point.y < m_maxs.y) &&
		(point.z > m_mins.z && point.z < m_maxs.z)
	);
}

/*! \brief Gets the center of the AABB3
* 
* \return A Vec3 containing the coordinates of the center of the AABB3
* 
*/
Vec3 const AABB3::GetCenter() const
{
	return Vec3(
		(m_maxs.x + m_mins.x) * 0.5f,
		(m_maxs.y + m_mins.y) * 0.5f,
		(m_maxs.z + m_mins.z) * 0.5f
	);
}

/*! \brief Gets the dimensions of the AABB3
* 
* \return A Vec3 containing the dimensions of the AABB3
* 
*/
Vec3 const AABB3::GetDimensions() const
{
	return Vec3(m_maxs.x - m_mins.x, m_maxs.y - m_mins.y, m_maxs.z - m_mins.z);
}

/*! \brief Gets the nearest point in the AABB3 to a given reference point
* 
* If the reference point is inside the AABB3, the nearest point is the reference point itself
* \param referencePoint A Vec3 containing the coordinates of the reference point for which the nearest point in the AABB3 should be determined
* \return A Vec3 containing the coordinates of the nearest point in the AABB3 to the given reference point
* 
*/
Vec3 const AABB3::GetNearestPoint(Vec3 const& referencePoint) const
{
	return Vec3(
		GetClamped(referencePoint.x, m_mins.x, m_maxs.x),
		GetClamped(referencePoint.y, m_mins.y, m_maxs.y),
		GetClamped(referencePoint.z, m_mins.z, m_maxs.z)
	);
}

/*! \brief Translates the AABB3 by the given translation
* 
* Does not change the dimensions of the AABB3.
* \param translationToApply A Vec3 representing the displacement from the current AABB3 position
* 
*/
void AABB3::Translate(Vec3 const& translationToApply)
{
	m_mins.x += translationToApply.x;
	m_maxs.x += translationToApply.x;
	m_mins.y += translationToApply.y;
	m_maxs.y += translationToApply.y;
	m_mins.z += translationToApply.z;
	m_maxs.z += translationToApply.z;
}

/*! \brief Sets the center of the AABB3 to the provided coordinates
* 
* Does not change the dimensions of the AABB3.
* \param newCenter A Vec3 containing the coordinates of the new center of the AABB3
* 
*/
void AABB3::SetCenter(Vec3 const& newCenter)
{
	float width = (m_maxs.x - m_mins.x);
	float height = (m_maxs.y - m_mins.y);
	float depth = (m_maxs.z - m_mins.z);
	m_mins.x = newCenter.x - width * 0.5f;
	m_maxs.x = newCenter.x + width * 0.5f;
	m_mins.y = newCenter.y - height * 0.5f;
	m_maxs.y = newCenter.y + height * 0.5f;
	m_mins.z = newCenter.z - depth * 0.5f;
	m_maxs.z = newCenter.z + depth * 0.5f;
}

/*! \brief Sets the dimensions of the AABB3 to the provided dimensions
* 
* Does not change the center of the AABB3- only stretches/squeezes the AABB3 from all directions to match the given dimensions.
* \param newDimensions A Vec3 representing the new dimensions of the AABB3
* 
*/
void AABB3::SetDimensions(Vec3 const& newDimensions)
{
	float deltaWidth = newDimensions.x - (m_maxs.x - m_mins.x);
	float deltaHeight = newDimensions.y - (m_maxs.y - m_mins.y);
	float deltaDepth = newDimensions.z - (m_maxs.z - m_mins.z);
	m_mins.x -= deltaWidth * 0.5f;
	m_maxs.x += deltaWidth * 0.5f;
	m_mins.y -= deltaHeight * 0.5f;
	m_maxs.y += deltaHeight * 0.5f;
	m_mins.z -= deltaDepth * 0.5f;
	m_maxs.z += deltaDepth * 0.5f;
}

/*! \brief Stretches the AABB3 to include a given point
* 
* Only stretches the AABB3 so that the given point is on the edge of the AABB3 and not inside. If the given point is already inside the AABB3, this method does nothing (does not squeeze the AABB3).
* \param point A Vec3 containing the coordinates of the point to include
* 
*/
void AABB3::StretchToIncludePoint(Vec3 const& point)
{
	if (point.x < m_mins.x)
	{
		m_mins.x = point.x;
	}
	else if (point.x > m_maxs.x)
	{
		m_maxs.x = point.x;
	}

	if (point.y < m_mins.y)
	{
		m_mins.y = point.y;
	}
	else if (point.y > m_maxs.y)
	{
		m_maxs.y = point.y;
	}

	if (point.z < m_mins.z)
	{
		m_mins.z = point.z;
	}
	else if (point.z > m_maxs.z)
	{
		m_maxs.z = point.z;
	}
}

void AABB3::GetCornerPoints(Vec3* out_cornerPoints) const
{
	out_cornerPoints[0] = m_mins;
	out_cornerPoints[1] = Vec3(m_mins.x, m_mins.y, m_maxs.z);
	out_cornerPoints[3] = Vec3(m_mins.x, m_maxs.y, m_mins.z);
	out_cornerPoints[2] = Vec3(m_mins.x, m_maxs.y, m_maxs.z);

	out_cornerPoints[4] = Vec3(m_maxs.x, m_mins.y, m_mins.z);
	out_cornerPoints[5] = Vec3(m_maxs.x, m_maxs.y, m_mins.z);
	out_cornerPoints[6] = Vec3(m_maxs.x, m_mins.y, m_maxs.z);
	out_cornerPoints[7] = m_maxs;
}
