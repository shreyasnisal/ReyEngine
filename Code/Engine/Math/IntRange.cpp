#include "Engine/Math/IntRange.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

/*! \brief Constructs an IntRange from the provided minimum and maximum values
* 
* \param min The lower bound for the integer range
* \param max The upper bound for the integer range
* 
*/
IntRange::IntRange(int min, int max)
	: m_min(min)
	, m_max(max)
{
}

/*! \brief Assignment operator for IntRange objects
* 
* Assigns the minimum and maximum values of the IntRange on the right side of the assignment operator to the IntRange on the left.
* \param intRangeToAssign The IntRange from which the minimum and maximum values should be obtained and set as the minimum and maximum values for this IntRange
* 
*/
void IntRange::operator=(IntRange const& intRangeToAssign)
{
	m_min = intRangeToAssign.m_min;
	m_max = intRangeToAssign.m_max;
}

/*! \brief Relational equal-to operator for IntRange objects
* 
* Two IntRange objects are equal iff they have the same lower and upper bounds.
* \param intRangeToCompare The IntRange with which this IntRange should be compared to check if they are equal
* \return A boolean indicating if the two IntRange objects are equal
* 
*/
bool IntRange::operator==(IntRange const& intRangeToCompare) const
{
	return (m_min == intRangeToCompare.m_min && m_max == intRangeToCompare.m_max);
}

/*! \brief Relational not-equals operator for IntRange objects
* 
* Two IntRange objects are not equal if either the lower or upper bound of one is not equal to that of the other.
* \param intRangeToCompare The IntRange with which this IntRange should be compared to check if they are not equal
* \return A boolean indicating if two IntRange obejcts are <b>not</b> equal
* 
*/
bool IntRange::operator!=(IntRange const& intRangeToCompare) const
{
	return (m_min != intRangeToCompare.m_min || m_max == intRangeToCompare.m_max);
}

/*! \brief Checks if the provided integer is on this IntRange
* 
* An integer is on an IntRange iff it is greater than the lower bound and less than the upper bound.
* \param value The integer value that should be checked for whether it is on thie IntRange
* \return A boolean indicating if the provided value is on this IntRange
* 
*/
bool IntRange::IsOnRange(int value) const
{
	return (value > m_min && value < m_max);
}

/*! \brief Checks if two IntRange objects have an overlap
* 
* \param otherIntRange The IntRange that should be compared with this IntRange to check for an overlap
* \return A boolean indicating if there is an overlap between the two IntRange objects
* 
*/
bool IntRange::IsOverlappingWith(IntRange const& otherIntRange) const
{
	return (
		IsOnRange(otherIntRange.m_min) ||
		IsOnRange(otherIntRange.m_max) ||
		otherIntRange.IsOnRange(m_min) ||
		otherIntRange.IsOnRange(m_max)
	);
}

/*! \brief Constructs the IntRange color from a string
*
* Minimum and maximum values for the IntRange must be separated by a '~'. Fatal error if the number of literals in the string are incorrect. If the number of literals is correct but any of the components cannot be parsed to an integer, that component is set to a value of 0.
* \param text The string to be parsed for the IntRange
*
*/
void IntRange::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, '~');

	if (numSplitStrings != 2)
	{
		ERROR_AND_DIE("Incorrect number of literals in IntRange string!");
	}

	m_min = atoi(splitStrings[0].c_str());
	m_max = atoi(splitStrings[1].c_str());
}
