#include "Engine/Math/AABB2.hpp"

#include "Engine/Math/MathUtils.hpp"

/*! \brief Constructs an AABB2 given two points representing the bottom left and top right corners
* 
* \param mins A Vec2 representing the bottom left corner of the box
* \param maxs A Vec2 representing the top right corner of the box
*/
AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
	: m_mins(mins)
	, m_maxs(maxs)
{
}

/*! \brief Constructs an AABB2 given four points representing the minimum XY and maximum XY values
* 
* \param minX The minimum X value (X value for the bottom left corner)
* \param minY The minimum Y value (Y value for the bottom left corner)
* \param maxX The minimum X value (X value for the top right corner)
* \param maxY The maximum Y value (Y value for the top right corner)
* 
*/
AABB2::AABB2(float minX, float minY, float maxX, float maxY)
	: m_mins(minX, minY)
	, m_maxs(maxX, maxY)
{
}

/*! \brief Gets whether a given point is inside the AABB2
* 
* A point is inside the AABB2 if its X value is between the minimum and maximum X for the AABB2 and its Y value is between the minimum and maximum Y for the AABB2.
* \param point A Vec2 representing the point for which the method should check if it is inside the AABB2
* \return A boolean indicating whether the point is inside the AABB2
*/
bool AABB2::IsPointInside(Vec2 const& point) const
{
	return (
		(point.x > m_mins.x && point.x < m_maxs.x) &&
		(point.y > m_mins.y && point.y < m_maxs.y)
	);
}

/*! \brief Gets the center of the AABB2
* 
* \return A Vec2 containing the coordinates of the center of the AABB2
* 
*/
Vec2 const AABB2::GetCenter() const
{
	return Vec2((m_maxs.x + m_mins.x) * 0.5f, (m_maxs.y + m_mins.y) * 0.5f);
}

/*! \brief Gets the dimensions of the AABB2
* 
* \return A Vec2 containing the dimensions of the AABB2
* 
*/
Vec2 const AABB2::GetDimensions() const
{
	return Vec2(m_maxs.x - m_mins.x, m_maxs.y - m_mins.y);
}

/*! \brief Gets the nearest point on the AABB2 to a provided reference point
* 
* If the reference point is inside the AABB2, the nearest point is the point itself.
* \param referencePoint A Vec2 containing the reference point for which the nearest point should be determined
* \return A Vec2 containing the point nearest point on the AABB2 to the reference point
* 
*/
Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePoint) const
{
	return Vec2(GetClamped(referencePoint.x, m_mins.x, m_maxs.x), GetClamped(referencePoint.y, m_mins.y, m_maxs.y));
}

/*! \brief Gets the point on the AABB2 corresponding to the provided UV coordinates
* 
* If the U or V coordinate is > 1.f, this method will return a point not on the AABB2
* \param uv A Vec2 containing the UV coordinates for which the point should be returned
* \return A Vec2 containing the coordinates of the point on the AABB2 corresponding to the provided UV coordinates
* 
*/
Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const
{
	return Vec2(m_mins.x + (m_maxs.x - m_mins.x) * uv.x, m_mins.y + (m_maxs.y - m_mins.y) * uv.y);
}

/*! \brief Gets the UV coordinates for a point on the AABB2
* 
* If the point is not on the AABB2, this method will return UV coordinates with U and/or V > 1.f.
* \param point A Vec2 containing the coordinates of a point on the AABB2 for which the UV coordinates should be returned
* \return A Vec2 containing the UV coordinates corresponding to the provided point on the AABB2
* 
*/
Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const
{
	return Vec2((point.x - m_mins.x) / (m_maxs.x - m_mins.x), (point.y - m_mins.y) / (m_maxs.y - m_mins.y));
}

/*! \brief Gets an AABB2 defined by the minimum and maximum UVs provided
* 
* \param uvAtMins A Vec2 representing the UV coordinates of the mins for the target box
* \param uvAtMaxs A Vec2 representing the UV coordinates of the maxs for the target box
* \return An AABB2 defined by the provided minimum and maximum UV coordinates
* 
*/
AABB2 const AABB2::GetBoxAtUVs(Vec2 const& uvMins, Vec2 const& uvMaxs) const
{
	return AABB2(GetPointAtUV(uvMins), GetPointAtUV(uvMaxs));
}

/*! \brief Translates an AABB2 by the provided translation
* 
* \param translationToApply The displacement to apply from the current position of the AABB2
* 
*/
void AABB2::Translate(Vec2 const& translationToApply)
{
	m_mins.x += translationToApply.x;
	m_maxs.x += translationToApply.x;
	m_mins.y += translationToApply.y;
	m_maxs.y += translationToApply.y;
}

/*! \brief Sets the center of the AABB2 to the specified position
* 
* Does not change the dimensions of the AABB2.
* \param newCenter A Vec2 containing the coordinates of the new center for the AABB2
* 
*/
void AABB2::SetCenter(Vec2 const& newCenter)
{
	float width = (m_maxs.x - m_mins.x);
	float height = (m_maxs.y - m_mins.y);
	m_mins.x = newCenter.x - width * 0.5f;
	m_maxs.x = newCenter.x + width * 0.5f;
	m_mins.y = newCenter.y - height * 0.5f;
	m_maxs.y = newCenter.y + height * 0.5f;
}

/*! \brief Sets the dimensions of the AABB2 to the specific dimensions
* 
* Does not change the center of the AABB2 (stretches/squeezes the AABB2 from all directions).
* \param newDimensions A Vec2 containing the new dimensions for the AABB2
* 
*/
void AABB2::SetDimensions(Vec2 const& newDimensions)
{
	float deltaWidth = newDimensions.x - (m_maxs.x - m_mins.x);
	float deltaHeight = newDimensions.y - (m_maxs.y - m_mins.y);
	m_mins.x -= deltaWidth * 0.5f;
	m_maxs.x += deltaWidth * 0.5f;
	m_mins.y -= deltaHeight * 0.5f;
	m_maxs.y += deltaHeight * 0.5f;
}

/*! \brief Stretches the AABB2 to include a given point
* 
* Only stretches the AABB2 so that the given point is on the edge of the AABB2 and not inside. If the given point is already inside the AABB2, this method does nothing (does not squeeze the AABB2).
* \param point The point that the AABB2 should stretch itself to include
* 
*/
void AABB2::StretchToIncludePoint(Vec2 const& point)
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
}

void AABB2::AddPadding(float xToAddOnBothSides, float yToAddOnBothSides)
{
	m_mins.x -= xToAddOnBothSides;
	m_maxs.x += xToAddOnBothSides;
	m_mins.y -= yToAddOnBothSides;
	m_maxs.y += yToAddOnBothSides;
}

void AABB2::ReduceToAspect(float targetAspect)
{
	Vec2 originalDimensions = GetDimensions();
	float originalAspect = originalDimensions.x / originalDimensions.y;

	if (targetAspect < originalAspect)
	{
		// Need to scale X only
		float targetWidth = originalDimensions.y * targetAspect;
		SetDimensions(Vec2(targetWidth, originalDimensions.y));
	}
	else if (targetAspect > originalAspect)
	{
		// Need to scale Y only
		float targetHeight = originalDimensions.x / targetAspect;
		SetDimensions(Vec2(originalDimensions.x, targetHeight));
	}
}
