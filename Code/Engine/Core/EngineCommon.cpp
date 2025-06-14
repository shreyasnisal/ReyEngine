#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//! \file EngineCommon.cpp

//! A global blackboard. Game or engine code can set key-value pairs in this blackboard which can then be read by game or engine code. Often useful for engine code to communicate with game code
NamedProperties g_gameConfigBlackboard;

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
	
	if (uint8PtrArr[0] == 0x78 && uint8PtrArr[1] == 0x56 && uint8PtrArr[2] == 0x34 && uint8PtrArr[3] == 0x12)
	{
		return BufferEndian::LITTLE;
	}

	ERROR_AND_DIE("Unsupported EndianMode!");
}

void ReverseShortBytesInPlace(uint8_t* bytes)
{
	uint16_t* bytesAsShort = reinterpret_cast<uint16_t*>(bytes);
	*bytesAsShort = ((*bytesAsShort & 0x00FF) << 8) |
					((*bytesAsShort & 0xFF00) >> 8);
}

void ReverseWordBytesInPlace(uint8_t* bytes)
{
	uint32_t* bytesAsWord = reinterpret_cast<uint32_t*>(bytes);
	*bytesAsWord = ((*bytesAsWord & 0x000000FF) << 24) |
				   ((*bytesAsWord & 0x0000FF00) << 8) |
				   ((*bytesAsWord & 0x00FF0000) >> 8) |
				   ((*bytesAsWord & 0xFF000000) >> 24 );
}

void ReverseDWordBytesInPlace(uint8_t* bytes)
{
	uint64_t* bytesAsDWord = reinterpret_cast<uint64_t*>(bytes);
	*bytesAsDWord = ((*bytesAsDWord & 0x00000000000000FFull) << 56) |
					((*bytesAsDWord & 0x000000000000FF00ull) << 40) |
					((*bytesAsDWord & 0x0000000000FF0000ull) << 24) |
					((*bytesAsDWord & 0x00000000FF000000ull) << 8) |
					((*bytesAsDWord & 0x000000FF00000000ull) >> 8) |
					((*bytesAsDWord & 0x0000FF0000000000ull) >> 24) |
					((*bytesAsDWord & 0x00FF000000000000ull) >> 40) |
					((*bytesAsDWord & 0xFF00000000000000ull) >> 56);
}
