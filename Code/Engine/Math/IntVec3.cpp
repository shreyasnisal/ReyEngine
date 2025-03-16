#include "Engine/Math/IntVec3.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"

#include <math.h>

/*! \brief Constructs an IntVec3 from the provided x, y and z values
*
* \param initialX The x coordinate value for the IntVec3
* \param intialY The y coordinate value for the IntVec3
* \param initialZ The z coordinate value for the IntVec3
*/
IntVec3::IntVec3(int initialX, int initialY, int initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{
}

/*! \brief Sets the x, y and z coordinate values for this IntVec3 from the provided string
*
* If the string has an incorrect number of comma-separated value, this method gives a fatal error. If the number of comma-separated values is correct but any of the values cannot be parsed to an integer, that component for the IntVec3 is set to zero.
* \param text A char* representing the string from which the IntVec3 should be created
*
*/
void IntVec3::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, ',');

	if (numSplitStrings != 3)
	{
		ERROR_AND_DIE("Incorrect number of literals in IntVec3 string!");
	}

	x = atoi(splitStrings[0].c_str());
	y = atoi(splitStrings[1].c_str());
	z = atoi(splitStrings[2].c_str());
}

/*! \brief Gets the length of this IntVec3 from the origin (0, 0, 0)
*
* \return A floating point number representing the length of this IntVec3 from the origin
*
*/
float IntVec3::GetLength() const
{
	return sqrtf((static_cast<float>(x) * static_cast<float>(x)) + (static_cast<float>(y) * static_cast<float>(y)) + (static_cast<float>(z) * static_cast<float>(z)));
}

/*! \brief Gets the Taxicab length (Manhattan distance) of this IntVec3 from the origin
*
* The taxicab length or Manhattan distance of an IntVec3 is defined as the number of units that this IntVec3 is away from the origin based on the ability to only move along the x, y or z axis at a time.
* \return An integer representing the taxicab length (Manhattan distance) of this IntVec3 from the origin
*
*/
int IntVec3::GetTaxicabLength() const
{
	return abs(x) + abs(y) + abs(z);
}

/*! \brief Gets the length squared of this IntVec3 from the origin (0, 0, 0)
*
* \return An integer representing the length squared of this IntVec3 from the origin
*
*/
int IntVec3::GetLengthSquared() const
{
	return (x * x) + (y * y) + (z * z);
}

/*! \brief Adds two IntVec3 objects
*
* This addition is performed component-wise (resultX = thisX + intVecToAddX, resultY = thisY + intVecToAddY).
* \param intVecToAdd The IntVec3 that should be added to this IntVec3 to compute the resulting IntVec3
* \return An IntVec3 that contains the sum of the two IntVec3
*
*/
IntVec3 const IntVec3::operator+(const IntVec3& intVecToAdd) const
{
	return IntVec3(x + intVecToAdd.x, y + intVecToAdd.y, z + intVecToAdd.z);
}

/*! \brief Subtracts two IntVec3 objects
*
* This subtraction is performed component-wise (resultX = thisX - intVecToSubtractX, resultY = thisY - intVecToSubtractY, resultZ = thisZ - intVecToSubtractZ).
* \param intVecToSubtract The IntVec3 that should be subtracted from this IntVec3 to compute the resulting IntVec3
* \return An IntVec3 that contains the difference between the two IntVec3
*
*/
IntVec3 const IntVec3::operator-(const IntVec3& intVecToSubtract) const
{
	return IntVec3(x - intVecToSubtract.x, y - intVecToSubtract.y, z - intVecToSubtract.z);
}

/*! \brief Unary minus operator for IntVec3. Returns IntVec3(-x, -y, -z)
*
* \return An IntVec3 that contains the additive inverse of this IntVec3
*
*/
IntVec3 const IntVec3::operator-() const
{
	return IntVec3(-x, -y, -z);
}

/*! \brief Gets an IntVec3 that is obtained by scaling this IntVec3 by a uniform scaling factor
*
*
*/
IntVec3 const IntVec3::operator*(int uniformScale) const
{
	return IntVec3(x * uniformScale, y * uniformScale, z * uniformScale);
}

IntVec3 const IntVec3::operator/(int inverseScale) const
{
	return IntVec3(x / inverseScale, y / inverseScale, z / inverseScale);
}

bool IntVec3::operator==(IntVec3 const& intVecToCompare) const
{
	return ((x == intVecToCompare.x) && (y == intVecToCompare.y) && (z == intVecToCompare.z));
}

bool IntVec3::operator!=(IntVec3 const& intVecToCompare) const
{
	return ((x != intVecToCompare.x) || (y != intVecToCompare.y) || (z != intVecToCompare.z));
}

void IntVec3::operator=(const IntVec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

void IntVec3::operator+=(const IntVec3& intVecToAdd)
{
	x += intVecToAdd.x;
	y += intVecToAdd.y;
	z += intVecToAdd.z;
}

void IntVec3::operator-=(const IntVec3& intVecToSubtract)
{
	x -= intVecToSubtract.x;
	y -= intVecToSubtract.y;
	z -= intVecToSubtract.z;
}

void IntVec3::operator*=(int uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

void IntVec3::operator/=(int inverseScale)
{
	x /= inverseScale;
	y /= inverseScale;
	z /= inverseScale;
}

Vec3 const IntVec3::GetAsVec3() const
{
	return Vec3((float)x, (float)y, (float)z);
}

IntVec3 const operator*(int uniformScale, IntVec3 const& intVecToScale)
{
	return IntVec3(intVecToScale.x * uniformScale, intVecToScale.y * uniformScale, intVecToScale.z * uniformScale);
}
