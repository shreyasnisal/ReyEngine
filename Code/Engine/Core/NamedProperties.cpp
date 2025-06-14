#include "Engine/Core/NamedProperties.hpp"


/*! \brief Adds all attributes of the given XmlElement to the NamedStrings instance
*
* \param element The XmlElement for which all attributes should be added to this NamedStrings instance
*
*/
void NamedProperties::PopulateFromXmlElementAttributes(XmlElement const& element)
{
	XmlAttribute const* attribute = element.FirstAttribute();

	while (attribute)
	{
		SetValue(attribute->Name(), std::string(attribute->Value()));
		attribute = attribute->Next();
	}
}

/*! \brief Gets the value for a given key as a boolean
*
* If no value corresponding to the given key is found or it does not match a boolean literal, the defaultValue is returned.
* \param keyName A string indicating the key for which the value should be returned
* \param defaultValue A boolean indicating the value to be returned if no value for the provided key is found
* \return A boolean with the value corresponding to the key or the defaultValue if a value is not found
*
*/
bool NamedProperties::GetValue(std::string const& keyName, bool defaultValue) const
{
	bool value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		
		// The value exists, as a bool, as a string/cstr or some completely other type
		TypedProperty<bool>* typedPropertyAsBool = dynamic_cast<TypedProperty<bool>*>(mapIter->second);
		if (typedPropertyAsBool)
		{
			value = typedPropertyAsBool->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				if (!_stricmp(typedPropertyAsStr->m_data.c_str(), "true"))
				{
					value = true;
				}
				else if (!_stricmp(typedPropertyAsStr->m_data.c_str(), "false"))
				{
					value = false;
				}
			}
			else
			{
				TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
				if (typedPropertyAsCStr)
				{
					if (!_stricmp(typedPropertyAsCStr->m_data, "true"))
					{
						value = true;
					}
					else if (!_stricmp(typedPropertyAsCStr->m_data, "false"))
					{
						value = false;
					}
				}
			}
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
int NamedProperties::GetValue(std::string const& keyName, int defaultValue) const
{
	int value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		TypedProperty<int>* typedPropertyAsInt = dynamic_cast<TypedProperty<int>*>(mapIter->second);
		if (typedPropertyAsInt)
		{
			value = typedPropertyAsInt->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				value = atoi(typedPropertyAsStr->m_data.c_str());
			}
			else
			{
				TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
				if (typedPropertyAsCStr)
				{
					value = atoi(typedPropertyAsCStr->m_data);
				}
			}
		}
	}
	return value;
}

unsigned char NamedProperties::GetValue(std::string const& keyName, unsigned char defaultValue) const
{
	char value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		TypedProperty<char>* typedPropertyAsChar = dynamic_cast<TypedProperty<char>*>(mapIter->second);
		if (typedPropertyAsChar)
		{
			value = typedPropertyAsChar->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				value = (unsigned char)atoi(typedPropertyAsStr->m_data.c_str());
			}
			else
			{
				TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
				if (typedPropertyAsCStr)
				{
					value = (unsigned char)atoi(typedPropertyAsCStr->m_data);
				}
			}
		}
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
float NamedProperties::GetValue(std::string const& keyName, float defaultValue) const
{
	float value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		TypedProperty<float>* typedPropertyAsInt = dynamic_cast<TypedProperty<float>*>(mapIter->second);
		if (typedPropertyAsInt)
		{
			value = typedPropertyAsInt->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				value = (float)atof(typedPropertyAsStr->m_data.c_str());
			}
			else
			{
				TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
				if (typedPropertyAsCStr)
				{
					value = (float)atoi(typedPropertyAsCStr->m_data);
				}
			}
		}
	}
	return value;
}

std::string NamedProperties::GetValue(std::string const& keyName, char const* defaultValue) const
{
	std::string value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
		if (typedPropertyAsCStr)
		{
			value = typedPropertyAsCStr->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				value = typedPropertyAsStr->m_data.c_str();
			}
		}
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
Rgba8 NamedProperties::GetValue(std::string const& keyName, Rgba8 const& defaultValue) const
{
	Rgba8 value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		TypedProperty<Rgba8>* typedPropertyAsRgba = dynamic_cast<TypedProperty<Rgba8>*>(mapIter->second);
		if (typedPropertyAsRgba)
		{
			value = typedPropertyAsRgba->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				value.SetFromText(typedPropertyAsStr->m_data.c_str());
			}
			else
			{
				TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
				if (typedPropertyAsCStr)
				{
					value.SetFromText(typedPropertyAsCStr->m_data);
				}
			}
		}
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
Vec2 NamedProperties::GetValue(std::string const& keyName, Vec2 const& defaultValue) const
{
	Vec2 value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		TypedProperty<Vec2>* typedPropertyAsVec2 = dynamic_cast<TypedProperty<Vec2>*>(mapIter->second);
		if (typedPropertyAsVec2)
		{
			value = typedPropertyAsVec2->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				value.SetFromText(typedPropertyAsStr->m_data.c_str());
			}
			else
			{
				TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
				if (typedPropertyAsCStr)
				{
					value.SetFromText(typedPropertyAsCStr->m_data);
				}
			}
		}
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
IntVec2 NamedProperties::GetValue(std::string const& keyName, IntVec2 const& defaultValue) const
{
	IntVec2 value = defaultValue;
	auto mapIter = m_properties.find(keyName);
	if (mapIter != m_properties.end())
	{
		TypedProperty<IntVec2>* typedPropertyAsIntVec2 = dynamic_cast<TypedProperty<IntVec2>*>(mapIter->second);
		if (typedPropertyAsIntVec2)
		{
			value = typedPropertyAsIntVec2->m_data;
		}
		else
		{
			TypedProperty<std::string>* typedPropertyAsStr = dynamic_cast<TypedProperty<std::string>*>(mapIter->second);
			if (typedPropertyAsStr)
			{
				value.SetFromText(typedPropertyAsStr->m_data.c_str());
			}
			else
			{
				TypedProperty<char const*>* typedPropertyAsCStr = dynamic_cast<TypedProperty<char const*>*>(mapIter->second);
				if (typedPropertyAsCStr)
				{
					value.SetFromText(typedPropertyAsCStr->m_data);
				}
			}
		}
	}
	return value;
}
