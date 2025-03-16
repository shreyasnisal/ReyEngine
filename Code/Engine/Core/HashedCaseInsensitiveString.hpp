#pragma once

#include <string>


class HashedCaseInsensitiveString
{
public:
	HashedCaseInsensitiveString() = default;
	HashedCaseInsensitiveString(HashedCaseInsensitiveString const& copyFrom) = default;
	HashedCaseInsensitiveString(char const* text);
	HashedCaseInsensitiveString(std::string text);

	static unsigned int GetHashForText(char const* text);
	static unsigned int GetHashForText(std::string const& text);

	unsigned int GetHash() const { return m_caseInsensitiveHash; }
	std::string const& GetOriginalString() const { m_originalStr; }
	char const* c_str() const { return m_originalStr.c_str(); }

	bool operator<(HashedCaseInsensitiveString const& hcisToCompare) const;
	bool operator>(HashedCaseInsensitiveString const& hcisToCompare) const;
	bool operator==(HashedCaseInsensitiveString const& hcisToCompare) const;
	bool operator!=(HashedCaseInsensitiveString const& hcisToCompare) const;
	bool operator==(char const* strToCompare) const;
	bool operator!=(char const* strToCompare) const;
	bool operator==(std::string strToCompare) const;
	bool operator!=(std::string strToCompare) const;
	void operator=(HashedCaseInsensitiveString const& assignFrom);
	void operator=(char const* text);
	void operator=(std::string const& text);

private:
	std::string m_originalStr;
	unsigned int m_caseInsensitiveHash = 0;
};
