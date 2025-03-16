#include "Engine/Core/XmlUtils.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB3.hpp"

/*! \brief Parse an XML attribute as an integer
* 
* Converts the value of an XML attribute to an integer and returns the integer, or returns the defaultValue integer passed in as an argument if the attribute is not found. If an attribute is found but its value cannot be parsed to an integer, returns 0.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue An integer representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return An integer representing the value of the provided attribute in the provided element, or the default value if the value is not found or 0 if a value is found but cannot be parsed to an integer
* 
*/
int	ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	int result = defaultValue;
	if (valueAsString)
	{
		result = atoi(valueAsString);
	}

	return result;
}

/*! \brief Parse an XML attribute as a character
*
* Converts the value of an XML attribute to a character and returns the character, or returns the defaultValue char passed in as an argument if the attribute is not found.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A char representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A char representing the value of the provided attribute in the provided element, or the default value if the attribute is not found
*
*/
char ParseXmlAttribute(XmlElement const& element, char const* attributeName, char defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	char result = defaultValue;
	if (valueAsString)
	{
		result = valueAsString[0];
	}

	return result;
}

/*! \brief Parse an XML attribute as a boolean
*
* Converts the value of an XML attribute to a boolean and returns the bool, or returns the defaultValue bool passed in as an argument if the attribute is not found or an attribute is found but its value cannot be parsed to a boolean.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A bool representing the value to be returned if an attribute with the provided name is not found in the provided element or if an attribute is found but its value cannot be parsed to a bool
* \return A bool representing the value of the provided attribute in the provided element, or the default value if the attribute is not found or the attribute is found but its value cannot be parsed to a bool
*
*/
bool ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	bool result = defaultValue;
	if (valueAsString)
	{
		if (!strcmp(valueAsString, "true") || !strcmp(valueAsString, ""))
		{
			result = true;
		}
		else if (!strcmp(valueAsString, "false"))
		{
			result = false;
		}
	}

	return result;
}

/*! \brief Parse an XML attribute as a floating point number
*
* Converts the value of an XML attribute to a floating point number and returns the float, or returns the defaultValue float passed in as an argument if the attribute is not found. If an attribute is found but its value cannot be parsed to a floating point number, returns 0.f.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A float representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A float representing the value of the provided attribute in the provided element, or the default value if the attribute is not found. If an attribute is found but its value cannot be parsed to a float, returns 0.f
*
*/
float ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	float result = defaultValue;
	if (valueAsString)
	{
		result = static_cast<float>(atof(valueAsString));
	}

	return result;
}

/*! \brief Parse an XML attribute as an Rgba8 color
*
* Converts the value of an XML attribute to an Rgba8 color and returns the Rgba8, or returns the defaultValue Rgba8 passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to Rgba8::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue An Rgba8 representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return An Rgba8 representing the value obtained using Rgba8::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa Rgba8::SetFromText
* 
*/
Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	Rgba8 result = defaultValue;
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}

/*! \brief Parse an XML attribute as a Vec2
*
* Converts the value of an XML attribute to a Vec2 and returns the Vec2, or returns the defaultValue Vec2 passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to Vec2::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A Vec2 representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A Vec2 representing the value obtained using Vec2::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa Vec2::SetFromText
* 
*/
Vec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	Vec2 result = defaultValue;
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}

/*! \brief Parse an XML attribute as a Vec3
*
* Converts the value of an XML attribute to a Vec3 and returns the Vec3, or returns the defaultValue Vec3 passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to Vec3::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A Vec3 representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A Vec3 representing the value obtained using Vec3::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa Vec3::SetFromText
*
*/
Vec3 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec3 const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	Vec3 result = defaultValue;
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}

/*! \brief Parse an XML attribute as an IntVec2
*
* Converts the value of an XML attribute to an IntVec2 and returns the IntVec2, or returns the defaultValue IntVec2 passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to IntVec2::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue An IntVec2 representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return An IntVec2 representing the value obtained using IntVec2::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa IntVec2::SetFromText
*
*/
IntVec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	IntVec2 result = defaultValue;
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}

/*! \brief Parse an XML attribute as a string
*
* Converts the value of an XML attribute to a string and returns the string, or returns the defaultValue string passed in as an argument if the attribute is not found.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A string representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A string containing the value of the provided attribute in the provided element, or the default value if the attribute is not found
*
*/
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	std::string result = defaultValue;
	if (valueAsString)
	{
		result = valueAsString;
	}
	return result;
}

/*! \brief Parse an XML attribute as a string
*
* Converts the value of an XML attribute to a string and returns the string, or returns the defaultValue char* (converted to string) passed in as an argument if the attribute is not found.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A char* representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A string containing the value of the provided attribute in the provided element, or the default value if the attribute is not found
*
*/
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	std::string result = defaultValue;
	if (valueAsString)
	{
		result = valueAsString;
	}
	return result;
}

/*! \brief Parse an XML attribute as a #Strings (list of strings) split based on the ',' delimiter
*
* Converts the value of an XML attribute to a list a strings split based on a ',' delimiter and returns the #Strings, or returns the defaultValue #Strings passed in as an argument if the attribute is not found.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A #Strings representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A #Strings (list of strings) split based on the ',' delimiter containing the value of the provided attribute in the provided element, or the default value if the attribute is not found
*
*/
Strings ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	Strings result = defaultValue;
	if (valueAsString)
	{
		SplitStringOnDelimiter(result, valueAsString, ',');
	}
	return result;
}

/*! \brief Parse an XML attribute as a FloatRange
*
* Converts the value of an XML attribute to a FloatRange and returns the FloatRange, or returns the defaultValue FloatRange passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to FloatRange::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue A FloatRange representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return A FloatRange representing the value obtained using FloatRange::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa FloatRange::SetFromText
*
*/
FloatRange ParseXmlAttribute(XmlElement const& element, char const* attributeName, FloatRange const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	FloatRange result = defaultValue;
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}

/*! \brief Parse an XML attribute as an IntRange
*
* Converts the value of an XML attribute to an IntRange and returns the IntRange, or returns the defaultValue IntRange passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to IntRange::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue An IntRange representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return An IntRange representing the value obtained using IntRange::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa IntRange::SetFromText
*
*/
IntRange ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntRange const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	IntRange result = defaultValue;
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}

/*! \brief Parse an XML attribute as an EulerAngles
* 
* Converts the value of an XML attribute to an EulerAngles and returns the EulerAngles, or returns the defaultValue EulerAngles passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to EulerAngles::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue An EulerAngles representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return An EulerAngles representing the value obtained using EulerAngles::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa EulerAngles::SetFromText
* 
*/
EulerAngles ParseXmlAttribute(XmlElement const& element, char const* attributeName, EulerAngles const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	EulerAngles result = defaultValue;
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}

/*! \brief Parse an XML attribute as an AABB3
*
* Converts the value of an XML attribute to an AABB3 and returns the AABB3, or returns the defaultValue AABB3 passed in as an argument if the attribute is not found. If an attribute is found, issues in parsing are handled according to AABB3::SetFromText.
* \param element The #XmlElement that attribute belongs (or should belong) to
* \param attributeName A char* representing the name of the attribute to parse
* \param defaultValue An AABB3 representing the value to be returned if an attribute with the provided name is not found in the provided element
* \return An AABB3 representing the value obtained using EulerAngles::SetFromText on the value of the provided attribute in the provided element, or the default value if the attribute is not found
* \sa AABB3::SetFromText
*
*/
AABB3 ParseXmlAttribute(XmlElement const& element, char const* attributeName, AABB3 const& defaultValue)
{
	char const* valueAsString = element.Attribute(attributeName);
	AABB3 result(defaultValue);
	if (valueAsString)
	{
		result.SetFromText(valueAsString);
	}
	return result;
}
