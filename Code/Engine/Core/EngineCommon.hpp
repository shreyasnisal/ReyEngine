#pragma once

#include "Engine/Core/NamedProperties.hpp"

#include <algorithm>
#include <cctype>
#include <string>


class DevConsole;
class EventSystem;
class InputSystem;
class OpenXR;
class UISystem;

#define UNUSED(x) (void)(x);

extern NamedProperties g_gameConfigBlackboard;
extern DevConsole* g_console;
extern EventSystem* g_eventSystem;
extern InputSystem* g_input;
extern OpenXR* g_openXR;
extern UISystem* g_ui;

enum class VertexType
{
	VERTEX_PCU,
	VERTEX_PCUTBN,
};

VertexType GetVertexTypeFromString(std::string vertexTypeStr);

enum class XREye
{
	NONE = -1,
	LEFT = 0,
	RIGHT = 1
};

enum class XRHand
{
	NONE = -1,
	LEFT,
	RIGHT,
};

enum class BufferEndian
{
	NATIVE = 0,
	LITTLE = 1,
	BIG = 2,
};

BufferEndian GetPlatformNativeEndianMode();

void ReverseShortBytesInPlace(uint8_t* bytes);
void ReverseWordBytesInPlace(uint8_t* bytes);
void ReverseDWordBytesInPlace(uint8_t* bytes);

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
