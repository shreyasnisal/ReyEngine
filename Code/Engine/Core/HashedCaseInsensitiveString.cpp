#include "Engine/Core/HashedCaseInsensitiveString.hpp"

HashedCaseInsensitiveString::HashedCaseInsensitiveString(char const* text)
	: m_originalStr(text)
	, m_caseInsensitiveHash(GetHashForText(text))
{
}

HashedCaseInsensitiveString::HashedCaseInsensitiveString(std::string text)
	: HashedCaseInsensitiveString(text.c_str())
{
}

unsigned int HashedCaseInsensitiveString::GetHashForText(char const* text)
{
	unsigned int hash = 0;

	for (char const* scan = text; *scan != '\0'; ++scan)
	{
		hash *= 31;
		hash += (unsigned int)std::tolower(*scan);
	}

	return hash;
}

unsigned int HashedCaseInsensitiveString::GetHashForText(std::string const& text)
{
	return GetHashForText(text.c_str());
}

bool HashedCaseInsensitiveString::operator<(HashedCaseInsensitiveString const& hcisToCompare) const
{
	return false;
}

bool HashedCaseInsensitiveString::operator>(HashedCaseInsensitiveString const& hcisToCompare) const
{
	return false;
}

bool HashedCaseInsensitiveString::operator==(HashedCaseInsensitiveString const& hcisToCompare) const
{
	if (m_caseInsensitiveHash != hcisToCompare.GetHash())
	{
		return false;
	}

	return m_originalStr == hcisToCompare.m_originalStr;
}

bool HashedCaseInsensitiveString::operator!=(HashedCaseInsensitiveString const& hcisToCompare) const
{
	return false;
}

bool HashedCaseInsensitiveString::operator==(char const* strToCompare) const
{
	return false;
}

bool HashedCaseInsensitiveString::operator!=(char const* strToCompare) const
{
	return false;
}

bool HashedCaseInsensitiveString::operator==(std::string strToCompare) const
{
	return false;
}

bool HashedCaseInsensitiveString::operator!=(std::string strToCompare) const
{
	return false;
}

void HashedCaseInsensitiveString::operator=(HashedCaseInsensitiveString const& assignFrom)
{
}

void HashedCaseInsensitiveString::operator=(char const* text)
{
	m_originalStr = text;
	m_caseInsensitiveHash = GetHashForText(text);
}

void HashedCaseInsensitiveString::operator=(std::string const& text)
{
	m_originalStr = text;
	m_caseInsensitiveHash = GetHashForText(text);
}
