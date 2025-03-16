#include "Engine/Math/FloatRange.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

/*! \brief Constructs a FloatRange from the given minimum and maximum values
* 
* \param min The lower bound for the FloatRange
* \param max The upper bound for the FloatRange
* 
*/
FloatRange::FloatRange(float min, float max)
	: m_min(min)
	, m_max(max)
{
}

/*! \brief Assignment operator for FloatRange objects
* 
* Assigns the minimum and maximum values of the FloatRange on the right side of the assignment operator to the FloatRange on the left.
* \param floatRangeToAssign The FloatRange from which the minimum and maximum values should be obtained and set as the minimum and maximum values for this FloatRange
* 
*/
void FloatRange::operator=(FloatRange const& floatRangeToAssign)
{
	m_min = floatRangeToAssign.m_min;
	m_max = floatRangeToAssign.m_max;
}

/*! \brief Relational equal-to operator for FloatRange objects
* 
* \warning This operator should be improved using a tolerance for the comparison to account for floating point imprecision.
* Two FloatRange objects are equal iff their minimum and maximum values are the same.
* \param floatRangeToCompare The FloatRange with which this FloatRange should be compared to check if the two are equal
* \return A boolean indicating whether the two FloatRange objects are equal to each other
* 
*/
bool FloatRange::operator==(FloatRange const& floatRangeToCompare) const
{
	return (m_min == floatRangeToCompare.m_min && m_max == floatRangeToCompare.m_max);
}

/*! \brief Relational not-equals operator for FloatRange objects
* 
* \warning This operator should be improved using a tolerance for the comparison to account for floating point imprecision.
* Two FloatRange objects are not equal iff either the minimum or the maximum of one is not equal to the minimum or maximum of the other.
* \param floatRangeToCompare The FloatRange with which this FloatRange should be compared to check if the two are not equal
* \return A boolean indicating whether the two FloatRange are <b>not</b> equal to each other
* 
*/
bool FloatRange::operator!=(FloatRange const& floatRangeToCompare) const
{
	return (m_min != floatRangeToCompare.m_min || m_max != floatRangeToCompare.m_max);
}

/*! \brief Checks if the provided value is on this FloatRange
* 
* The value is on the FloatRange iff it is greater than or equal to the maximum and less than or equal to the minimum
* \param value The float value for which the method should check if it is on the range
* \return A boolean indicating if the value is on this FloatRange
* 
*/
bool FloatRange::IsOnRange(float value) const
{
	return (value >= m_min && value <= m_max);
}

/*! \brief Checks if two FloatRange objects have an overlap
* 
* \param otherFloatRange The FloatRange that should be compared with this FloatRange to check for an overlap
* \return A boolean indicating whether there is an overlap in the two FloatRange objects
* 
*/
bool FloatRange::IsOverlappingWith(FloatRange const& otherFloatRange) const
{
	return (
		IsOnRange(otherFloatRange.m_min) ||
		IsOnRange(otherFloatRange.m_max) ||
		otherFloatRange.IsOnRange(m_min) ||
		otherFloatRange.IsOnRange(m_max)
	);
}

/*! \brief Constructs the FloatRange color from a string
*
* Minimum and maximum values for the FloatRange must be separated by a '~'. Fatal error if the number of literals in the string are incorrect. If the number of comma-separated components is correct but any of the components cannot be parsed to a float, that component is set to a value of 0.f.
* \param text The string to be parsed for the FloatRange
*
*/
void FloatRange::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, '~');

	if (numSplitStrings != 2)
	{
		ERROR_AND_DIE("Incorrect number of literals in FloatRange string!");
	}

	m_min = static_cast<float>(atof(splitStrings[0].c_str()));
	m_max = static_cast<float>(atof(splitStrings[1].c_str()));
}
