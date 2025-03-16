#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"


//! \file EngineCommon.cpp

//! A global blackboard. Game or engine code can set key-value pairs in this blackboard which can then be read by game or engine code. Often useful for engine code to communicate with game code
NamedStrings g_gameConfigBlackboard;

//! A global EventSystem instance. Although defined here, it must be initialized by game code before it can be used
EventSystem* g_eventSystem = nullptr;

//! A global DevConsole instance. Although defined here, it must be initialized by game code before it can be used
DevConsole* g_console = nullptr;

//! A global InputSystem instance. Although defined here, it must be initialized by game code before it can be used
InputSystem* g_input = nullptr;

OpenXR* g_openXR = nullptr;

UISystem* g_ui = nullptr;

VertexType GetVertexTypeFromString(std::string vertexTypeStr)
{
	if (!strcmp(vertexTypeStr.c_str(), "Vertex_PCUTBN"))
	{
		return VertexType::VERTEX_PCUTBN;
	}

	return VertexType::VERTEX_PCU;
}

BufferEndian GetPlatformNativeEndianMode()
{
	uint32_t uint32Ptr = 0x12345678u;
	uint8_t* uint8PtrArr = reinterpret_cast<uint8_t*>(&uint32Ptr);
	if (uint8PtrArr[0] == 0x12 && uint8PtrArr[1] == 0x34 && uint8PtrArr[2] == 0x56 && uint8PtrArr[3] == 0x78)
	{
		return BufferEndian::BIG;
	}
	
	if (uint8PtrArr[0] == 0x87 && uint8PtrArr[1] == 0x65 && uint8PtrArr[2] == 0x43 && uint8PtrArr[3] == 0x21)
	{
		return BufferEndian::LITTLE;
	}

	ERROR_AND_DIE("Unsupported native EndianMode!");
}
