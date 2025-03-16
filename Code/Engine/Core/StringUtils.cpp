#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
constexpr int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
std::string const Stringf( char const* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
std::string const Stringf( int maxLength, char const* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

int SplitStringOnDelimiter(Strings& out_splitStrings, std::string const& originalString, char delimiterToSplitOn, bool removeEmpty)
{
	int numStrings = 0;
	std::string splitString = "";

	for (int i = 0; i < static_cast<int>(originalString.length()); i++)
	{
		if (originalString[i] != delimiterToSplitOn)
		{
			splitString += originalString[i];
		}
		else
		{
			TrimString(splitString);
			if (removeEmpty && splitString.empty())
			{
				splitString = "";
				continue;
			}
			out_splitStrings.push_back(splitString);
			numStrings++;
			splitString = "";
		}
	}

	numStrings++;
	out_splitStrings.push_back(splitString);

	return numStrings;
}

int SplitStringOnDelimiter(Strings& out_splitStrings, std::string const& originalString, char delimiterToSplitOn, char characterToTokenizeOn)
{
	int numStrings = 0;
	std::string splitString = "";
	bool isInToken = false;

	for (int i = 0; i < static_cast<int>(originalString.length()); i++)
	{
		if (originalString[i] == characterToTokenizeOn)
		{
			isInToken = !isInToken;
		}
		else if (isInToken || originalString[i] != delimiterToSplitOn)
		{
			splitString += originalString[i];
		}
		else
		{
			out_splitStrings.push_back(splitString);
			numStrings++;
			splitString = "";
		}
	}

	numStrings++;
	out_splitStrings.push_back(splitString);

	return numStrings;
}

void TrimString(std::string& stringToTrim)
{
	if (stringToTrim.empty())
	{
		return;
	}

	// trim leading
	char stringCharacter = stringToTrim[0];
	int characterIndex = 0;

	while (stringCharacter == ' ' || stringCharacter == '\n' || stringCharacter == '\t' || stringCharacter == '\r')
	{
		stringToTrim.erase(characterIndex);

		if (stringToTrim.empty())
		{
			return;
		}
	}

	// trim trailing
	stringCharacter = stringToTrim.back();
	characterIndex = (int)stringToTrim.size() - 1;
	while (stringCharacter == ' ' || stringCharacter == '\n' || stringCharacter == '\t' || stringCharacter == '\r')
	{
		stringToTrim.erase(characterIndex);
		stringCharacter = stringToTrim.back();
		characterIndex = (int)stringToTrim.size() - 1;

		if (stringToTrim.empty())
		{
			return;
		}
	}
}

void StripString(std::string& stringToStrip, char tokenToStripOff)
{
	for (int charIndex = 0; charIndex < stringToStrip.length(); charIndex++)
	{
		if (stringToStrip[charIndex] == tokenToStripOff)
		{
			stringToStrip.erase(stringToStrip.begin() + charIndex);
			charIndex--;
		}
	}
}
