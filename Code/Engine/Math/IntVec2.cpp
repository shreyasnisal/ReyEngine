#include "Engine/Math/IntVec2.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

#include <math.h>

/*! \brief Constructs an IntVec2 from the provided x and y values
* 
* \param initialX The x coordinate value for the IntVec2
* \param intialY The y coordinate value for the IntVec2
*/
IntVec2::IntVec2(int initialX, int initialY)
	: x(initialX)
	, y(initialY)
{
}

/*! \brief Sets the x and y coordinate values for this IntVec2 from the provided string
* 
* If the string has an incorrect number of comma-separated value, this method gives a fatal error. If the number of comma-separated values is correct but any of the values cannot be parsed to an integer, that component for the IntVec2 is set to zero.
* \param text A char* representing the string from which the IntVec2 should be created
* 
*/
void IntVec2::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, ',');

	if (numSplitStrings != 2)
	{
		ERROR_AND_DIE("Incorrect number of literals in IntVec2 string!");
	}
	
	x = atoi(splitStrings[0].c_str());
	y = atoi(splitStrings[1].c_str());
}

/*! \brief Gets the length of this IntVec2 from the origin (0, 0)
* 
* \return A floating point number representing the length of this IntVec2 from the origin
* 
*/
float IntVec2::GetLength() const
{
	return sqrtf((static_cast<float>(x) * static_cast<float>(x)) + (static_cast<float>(y) * static_cast<float>(y)));
}

/*! \brief Gets the Taxicab length (Manhattan distance) of this IntVec2 from the origin
* 
* The taxicab length or Manhattan distance of an IntVec2 is defined as the number of units that this IntVec2 is away from the origin based on the ability to only move along the x or y axis at a time.
* \return An integer representing the taxicab length (Manhattan distance) of this IntVec2 from the origin
* 
*/
int IntVec2::GetTaxicabLength() const
{
	return abs(x) + abs(y);
}

/*! \brief Gets the length squared of this IntVec2 from the origin (0, 0)
* 
* \return An integer representing the length squared of this IntVec2 from the origin
* 
*/
int IntVec2::GetLengthSquared() const
{
	return (x * x) + (y * y);
}

/*! \brief Gets the orientation in radians of this IntVec2 from the +X axis
* 
* \return A floating point number representing the orientation in radians of this IntVec2 from the +X axis
* 
*/
float IntVec2::GetOrientationRadians() const
{
	return atan2f(static_cast<float>(y), static_cast<float>(x));
}

/*! \brief Gets the orientation in degrees of this IntVec2 from the +X axis
* 
* \return A floating point number representing the orientation in degrees of this IntVec2 from the +X axis
* 
*/
float IntVec2::GetOrientationDegrees() const
{
	return Atan2Degrees(static_cast<float>(y), static_cast<float>(x));
}

/*! \brief Gets an IntVec2 rotated +90 degrees from this IntVec2
* 
* This rotation uses a trick that only works for +/- 90 degree rotations to compute the resulting IntVec2 faster.
* \return An IntVec2 rotated +90 degrees (counter-clockwise) from this IntVec2
* 
*/
IntVec2 const IntVec2::GetRotated90Degrees() const
{
	return IntVec2(-y, x);
}

/*! \brief Gets an IntVec2 rotated -90 degrees from this IntVec2
* 
* This rotation uses a trick that only works for +/- 90 degree rotations to compute the resulting IntVec2 faster.
* \return An IntVec2 rotated -90 degrees (clockwise) from this IntVec2
* 
*/
IntVec2 const IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2(y, -x);
}

/*! \brief Adds two IntVec2 objects
* 
* This addition is performed component-wise (resultX = thisX + intVecToAddX, resultY = thisY + intVecToAddY).
* \param intVecToAdd The IntVec2 that should be added to this IntVec2 to compute the resulting IntVec2
* \return An IntVec2 that contains the sum of the two IntVec2
* 
*/
IntVec2 const IntVec2::operator+(const IntVec2& intVecToAdd) const
{
	return IntVec2(x + intVecToAdd.x, y + intVecToAdd.y);
}

/*! \brief Subtracts two IntVec2 objects
*
* This subtraction is performed component-wise (resultX = thisX - intVecToSubtractX, resultY = thisY - intVecToSubtractY).
* \param intVecToSubtract The IntVec2 that should be subtracted from this IntVec2 to compute the resulting IntVec2
* \return An IntVec2 that contains the difference between the two IntVec2
*
*/
IntVec2 const IntVec2::operator-(const IntVec2& intVecToSubtract) const
{
	return IntVec2(x - intVecToSubtract.x, y - intVecToSubtract.y);
}

/*! \brief Unary minus operator for IntVec2. Returns IntVec2(-x, -y)
* 
* \return An IntVec2 that contains the additive inverse of this IntVec2
* 
*/
IntVec2 const IntVec2::operator-() const
{
	return IntVec2(-x, -y);
}

/*! \brief Gets an IntVec2 that is obtained by scaling this IntVec2 by a uniform scaling factor
* 
* 
*/
IntVec2 const IntVec2::operator*(int uniformScale) const
{
	return IntVec2(x * uniformScale, y * uniformScale);
}

IntVec2 const IntVec2::operator/(int inverseScale) const
{
	return IntVec2(x / inverseScale, y / inverseScale);
}

bool IntVec2::operator==(IntVec2 const& intVecToCompare) const
{
	return ((x == intVecToCompare.x) && (y == intVecToCompare.y));
}

bool IntVec2::operator!=(IntVec2 const& intVecToCompare) const
{
	return ((x != intVecToCompare.x) || (y != intVecToCompare.y));
}

void IntVec2::Rotate90Degrees()
{
	int tempX = x;
	x = -y;
	y = tempX;
}

void IntVec2::RotateMinus90Degrees()
{
	int tempY = y;
	y = -x;
	x = tempY;
}

void IntVec2::operator=(const IntVec2& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
}

void IntVec2::operator+=(const IntVec2& intVecToAdd)
{
	x += intVecToAdd.x;
	y += intVecToAdd.y;
}

void IntVec2::operator-=(const IntVec2& intVecToSubtract)
{
	x -= intVecToSubtract.x;
	y -= intVecToSubtract.y;
}

void IntVec2::operator*=(int uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
}

void IntVec2::operator/=(int inverseScale)
{
	x /= inverseScale;
	y /= inverseScale;
}

Vec2 const IntVec2::GetAsVec2() const
{
	return Vec2((float)x, (float)y);
}

IntVec2 const operator*(int uniformScale, IntVec2 const& intVecToScale)
{
	return IntVec2(intVecToScale.x * uniformScale, intVecToScale.y * uniformScale);
}
