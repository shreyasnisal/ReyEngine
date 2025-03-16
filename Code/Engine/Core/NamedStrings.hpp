#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>


//! \cond
// Hides this struct from doxygen documentation
struct cmpCaseInsensitive
{
	bool operator() (std::string const& a, std::string const& b) const
	{
		std::string lowerCaseA = a;
		std::string lowerCaseB = b;
		std::transform(a.begin(), a.end(), lowerCaseA.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(static_cast<int>(c))); });
		std::transform(b.begin(), b.end(), lowerCaseB.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(static_cast<int>(c))); });
		return lowerCaseA < lowerCaseB;
	}
};
//! \endcond

/*! \brief A mapping of string-string key-value pairs
* 
* Although the class only offers a mapping of strings to strings, it provides convenience methods for retrieving values as different types.
* This is especially useful for reading data from XML files or to be used as a blackboard for communication between Engine and Game code.
* \sa g_gameConfigBlackboard
* 
*/
class NamedStrings
{
public:
	//! Default destructor for a NamedStrings instance
	~NamedStrings() = default;
	//! Default constructor for a NamedStrings instance
	NamedStrings() = default;

	void				PopulateFromXmlElementAttributes(XmlElement const& element);
	void				SetValue(std::string const& keyName, std::string const& newValue);
	std::string			GetValue(std::string const& keyName, std::string const& defaultValue) const;
	bool				GetValue(std::string const& keyName, bool defaultValue) const;
	int					GetValue(std::string const& keyName, int defaultValue) const;
	float				GetValue(std::string const& keyName, float defaultValue) const;
	std::string			GetValue(std::string const& keyName, char const* defaultValue) const;
	Rgba8				GetValue(std::string const& keyName, Rgba8 const& defaultValue) const;
	Vec2				GetValue(std::string const& keyName, Vec2 const& defaultValue) const;
	IntVec2				GetValue(std::string const& keyName, IntVec2 const& defaultValue) const;

private:
	//! The map that stores key-value pairs in the form of strings (names) and strings (values)
	std::map<std::string, std::string, cmpCaseInsensitive>		m_keyValuePairs;
};
