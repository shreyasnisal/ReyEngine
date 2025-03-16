#pragma once

#include "Engine/Core/NamedStrings.hpp"

#include <string>

class DevConsole;
class EventSystem;
class InputSystem;
class OpenXR;
class UISystem;

#define UNUSED(x) (void)(x);

extern NamedStrings g_gameConfigBlackboard;
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
	NATIVE,
	LITTLE,
	BIG,
};

BufferEndian GetPlatformNativeEndianMode();

