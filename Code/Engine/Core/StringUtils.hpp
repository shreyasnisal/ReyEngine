#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
std::string const Stringf( char const* format, ... );
std::string const Stringf( int maxLength, char const* format, ... );

int SplitStringOnDelimiter(Strings& out_splitStrings, std::string const& originalString, char delimiterToSplitOn, bool removeEmpty = false);
int SplitStringOnDelimiter(Strings& out_splitStrings, std::string const& originalString, char delimiterToSplitOn, char characterToTokenizeOn);
void TrimString(std::string& stringToTrim);
void StripString(std::string& stringToStrip, char tokenToStripOff);
