#pragma once

#include "Engine/Core/StringUtils.hpp"

#include <ThirdParty/TinyXML2/tinyxml2.h>

struct Rgba8;
struct IntVec2;
struct Vec2;
struct Vec3;
struct FloatRange;
struct IntRange;
class EulerAngles;
struct AABB3;

/*! \file XmlUtils.hpp
* \brief Contains helper functions for XML parsing
* 
* Uses the tinyxml2 library for XML parsing and provides aliases and helper functions to parse XML elements.
* 
*/

//! Alias for tinyxml2 XMLDocument
typedef tinyxml2::XMLDocument	XmlDocument;
//! Alias for tinyxml2 XMLElement
typedef tinyxml2::XMLElement	XmlElement;
//! Alias for tinyxml2 XMLAttribute
typedef tinyxml2::XMLAttribute	XmlAttribute;
//! Alias for tinyxml2 XMLError
typedef tinyxml2::XMLError		XmlResult;

int				ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue);
char			ParseXmlAttribute(XmlElement const& element, char const* attributeName, char defaultValue);
bool			ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue);
float			ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue);
Rgba8			ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue);
Vec2			ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue);
Vec3			ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec3 const& defaultValue);
IntVec2			ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue);
std::string		ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue);
std::string		ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue);
Strings			ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValue);
FloatRange		ParseXmlAttribute(XmlElement const& element, char const* attributeName, FloatRange const& defaultValue);
IntRange		ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntRange const& defaultValue);
EulerAngles		ParseXmlAttribute(XmlElement const& element, char const* attributeName, EulerAngles const& defaultValue);
AABB3			ParseXmlAttribute(XmlElement const& element, char const* attributeName, AABB3 const& defaultValue);
