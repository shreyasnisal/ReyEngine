#include "Engine/Core/NamedStrings.hpp"


/*! \brief Adds all attributes of the given XmlElement to the NamedStrings instance
* 
* \param element The XmlElement for which all attributes should be added to this NamedStrings instance
* 
*/
void NamedStrings::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	XmlAttribute const* attribute = element.FirstAttribute();

	while (attribute)
	{
		SetValue(attribute->Name(), attribute->Value());
		attribute = attribute->Next();
	}
}

/*! \brief Sets the value for a given key
* 
* If no key with the keyName exists, a new key-value mapping is added. If the key already exists, updates the value for that key.
* \param keyName A string indicating the name of the key for which the value should be added/updated
* \param newValue A string indicating the new value to be stored in the provided key
* 
*/
void NamedStrings::SetValue(std::string const& keyName, std::string const& newValue)
{
	m_keyValuePairs[keyName] = newValue;
}

/*! \brief Gets the value for a given key as a string
* 
* If no value corresponding to the given key is found, the defaultValue is returned.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue A string indicating the value to be returned if no value for the provided key is found
* \return A string with the value corresponding to the key or the defaultValue if a value is not found
* 
*/
std::string NamedStrings::GetValue(std::string const& keyName, std::string const& defaultValue) const
{
	std::string value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		value = mapIter->second;
	}
	return value;
}

/*! \brief Gets the value for a given key as a boolean
*
* If no value corresponding to the given key is found or it does not match a boolean literal, the defaultValue is returned.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue A boolean indicating the value to be returned if no value for the provided key is found
* \return A boolean with the value corresponding to the key or the defaultValue if a value is not found
*
*/
bool NamedStrings::GetValue(std::string const& keyName, bool defaultValue) const
{
	bool value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		if (!strcmp((mapIter->second).c_str(), "true"))
		{
			value = true;
		}
		else if (!strcmp((mapIter->second).c_str(), "false"))
		{
			value = false;
		}
	}
	return value;
}

/*! \brief Gets the value for a given key as an integer
*
* If no value corresponding to the given key is found, the defaultValue is returned. If the value corresponding to the key cannot be parsed to an integer, returns 0.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue An integer indicating the value to be returned if no value for the provided key is found
* \return An integer with the value corresponding to the key or the defaultValue if a value is not found or 0 if the value corresponding to the key cannot be parsed to an integer
*
*/
int NamedStrings::GetValue(std::string const& keyName, int defaultValue) const
{
	int value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		value = atoi((mapIter->second).c_str());
	}
	return value;
}

/*! \brief Gets the value for a given key as a floating point number
*
* If no value corresponding to the given key is found, the defaultValue is returned. If the value corresponding to the key cannot be parsed to a floating point number, returns 0.0f.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue A floating point number indicating the value to be returned if no value for the provided key is found
* \return A floating point number with the value corresponding to the key or the defaultValue if a value is not found or 0.0f if the value corresponding to the key cannot be parsed to a floating point number
*
*/
float NamedStrings::GetValue(std::string const& keyName, float defaultValue) const
{
	float value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		value = static_cast<float>(atof((mapIter->second).c_str()));
	}
	return value;
}

/*! \brief Gets the value for a given key as a string
*
* If no value corresponding to the given key is found, the defaultValue is returned.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue A character pointer indicating the value to be returned if no value for the provided key is found
* \return A string with the value corresponding to the key or the defaultValue (as a string) if a value is not found
*
*/
std::string NamedStrings::GetValue(std::string const& keyName, char const* defaultValue) const
{
	std::string value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		value = mapIter->second;
	}
	return value;
}

/*! \brief Gets the value for a given key as an Rgba8 color
*
* If no value corresponding to the given key is found, the defaultValue is returned. Fatal error if the value corresponding to the key cannot be parsed to an Rgba8 object due to an incorrect number of comma-separated components. If the value contains the correct number of comma-separated values but any of those values cannot be parsed to an integer, that component is set to 0.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue An Rgba8 color containing the color to be returned if no value for the provided key is found
* \return An Rgba8 color corresponding to the key or the defaultValue if a value is not found
*
*/
Rgba8 NamedStrings::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	Rgba8 value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		value.SetFromText((mapIter->second).c_str());
	}
	return value;
}

/*! \brief Gets the value for a given key as an Vec2
*
* If no value corresponding to the given key is found, the defaultValue is returned. Fatal error if the value corresponding to the key cannot be parsed to a Vec2 due to an incorrect number of comma-separated components. If the value contains the correct number of comma-separated components but any component cannot be parsed to a floating point number, that component is set to 0.0f.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue A Vec2 containing the vector to be returned if no value for the provided key is found
* \return A Vec2 corresponding to the key or the defaultValue if a value is not found
*
*/
Vec2 NamedStrings::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	Vec2 value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		value.SetFromText((mapIter->second).c_str());
	}
	return value;
}

/*! \brief Gets the value for a given key as an IntVec2
*
* If no value corresponding to the given key is found, the defaultValue is returned. Fatal error if the value corresponding to the key cannot be parsed to an IntVec2 due to an incorrect number of comma-separated components. If the value contains the correct number of comma-separated values but any of those values cannot be parsed to an integer, that component is set to 0.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue An IntVec2 containing the value to be returned if no value for the provided key is found
* \return An IntVec2 corresponding to the key or the defaultValue if a value is not found
*
*/
IntVec2 NamedStrings::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	IntVec2 value = defaultValue;
	auto mapIter = m_keyValuePairs.find(keyName);
	if (mapIter != m_keyValuePairs.end())
	{
		value.SetFromText((mapIter->second).c_str());
	}
	return value;
}
