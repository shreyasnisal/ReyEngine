#pragma once

#include "Engine/Core/HashedCaseInsensitiveString.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <map>
#include <string>
#include <typeinfo>


class TypedPropertyBase
{
public:
	virtual ~TypedPropertyBase() = default;
	TypedPropertyBase() = default;
};

template<typename T>
class TypedProperty : public TypedPropertyBase
{
	friend class NamedProperties;

	T m_data;
};

class NamedProperties
{
public:
	~NamedProperties() = default;
	NamedProperties() = default;

	void				PopulateFromXmlElementAttributes(XmlElement const& element);
	bool				GetValue(std::string const& keyName, bool defaultValue) const;
	int					GetValue(std::string const& keyName, int defaultValue) const;
	unsigned char		GetValue(std::string const& keyName, unsigned char defaultValue) const;
	float				GetValue(std::string const& keyName, float defaultValue) const;
	std::string			GetValue(std::string const& keyName, char const* defaultValue) const;
	Rgba8				GetValue(std::string const& keyName, Rgba8 const& defaultValue) const;
	Vec2				GetValue(std::string const& keyName, Vec2 const& defaultValue) const;
	IntVec2				GetValue(std::string const& keyName, IntVec2 const& defaultValue) const;

	template<typename T>
	T GetValue(std::string key, T defaultValue) const
	{
		auto mapIter = m_properties.find(key);
		if (mapIter == m_properties.end())
		{
			return defaultValue;
		}

		// There is a value stored for that key
		TypedProperty<T>* typedProperty = dynamic_cast<TypedProperty<T>*>(mapIter->second);
		if (!typedProperty)
		{
			return defaultValue;
		}

		return typedProperty->m_data;
	}
	
	template<typename T>
	void SetValue(std::string key, T value)
	{
		TypedProperty<T>* typedProperty = new TypedProperty<T>();
		typedProperty->m_data = value;
		m_properties[HCIS(key)] = typedProperty;
	}

public:
	std::map<HCIS, TypedPropertyBase*> m_properties;
};
