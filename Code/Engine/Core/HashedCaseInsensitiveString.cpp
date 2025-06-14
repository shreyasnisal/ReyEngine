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
	if (m_caseInsensitiveHash < hcisToCompare.m_caseInsensitiveHash)
	{
		return true;
	}

	if (m_caseInsensitiveHash > hcisToCompare.m_caseInsensitiveHash)
	{
		return false;
	}

	return _stricmp(m_originalStr.c_str(), hcisToCompare.m_originalStr.c_str()) < 0 ? true : false;
}

bool HashedCaseInsensitiveString::operator>(HashedCaseInsensitiveString const& hcisToCompare) const
{
	if (m_caseInsensitiveHash > hcisToCompare.m_caseInsensitiveHash)
	{
		return true;
	}

	if (m_caseInsensitiveHash < hcisToCompare.m_caseInsensitiveHash)
	{
		return false;
	}

	return _stricmp(m_originalStr.c_str(), hcisToCompare.m_originalStr.c_str()) > 0 ? true : false;
}

bool HashedCaseInsensitiveString::operator==(HashedCaseInsensitiveString const& hcisToCompare) const
{
	if (m_caseInsensitiveHash != hcisToCompare.m_caseInsensitiveHash)
	{
		return false;
	}

	return _stricmp(m_originalStr.c_str(), hcisToCompare.m_originalStr.c_str()) == 0 ? true : false;
}

bool HashedCaseInsensitiveString::operator!=(HashedCaseInsensitiveString const& hcisToCompare) const
{
	if (m_caseInsensitiveHash != hcisToCompare.m_caseInsensitiveHash)
	{
		return true;
	}

	return _stricmp(m_originalStr.c_str(), hcisToCompare.m_originalStr.c_str()) != 0 ? true : false;
}

bool HashedCaseInsensitiveString::operator==(char const* strToCompare) const
{
	return m_caseInsensitiveHash == GetHashForText(strToCompare);
}

bool HashedCaseInsensitiveString::operator!=(char const* strToCompare) const
{
	if (m_caseInsensitiveHash == GetHashForText(strToCompare))
	{
		return false;
	}

	return _stricmp(m_originalStr.c_str(), strToCompare) != 0 ? true : false;
}

bool HashedCaseInsensitiveString::operator==(std::string strToCompare) const
{
	return m_caseInsensitiveHash == GetHashForText(strToCompare);
}

bool HashedCaseInsensitiveString::operator!=(std::string strToCompare) const
{
	if (m_caseInsensitiveHash == GetHashForText(strToCompare))
	{
		return false;
	}

	return _stricmp(m_originalStr.c_str(), strToCompare.c_str()) != 0 ? true : false;
}

void HashedCaseInsensitiveString::operator=(HashedCaseInsensitiveString const& assignFrom)
{
	m_caseInsensitiveHash = assignFrom.m_caseInsensitiveHash;
	m_originalStr = assignFrom.m_originalStr;
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
