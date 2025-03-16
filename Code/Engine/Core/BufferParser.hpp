#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

#include <string>
#include <vector>


class BufferParser
{
public:
	BufferParser(std::vector<uint8_t>& buffer);

	void SetEndianMode(BufferEndian endianMode);
	BufferEndian GetEndianMode() const { return m_endianMode; }

	unsigned char ParseChar();
	unsigned char ParseByte();
	bool ParseBool();
	uint32_t ParseUint32();
	int32_t ParseInt32();
	float ParseFloat();
	double ParseDouble();
	void ParseStringZeroTerminated(std::string& out_string);
	void ParseStringAfter32BitLength(std::string& out_string);

	Rgba8 const ParseRgba();
	Rgba8 const ParseRgb();
	IntVec2 const ParseIntVec2();
	Vec2 const ParseVec2();
	Vec3 const ParseVec3();
	EulerAngles const ParseEulerAngles();
	Vertex_PCU const ParseVertexPCU();

	int GetRemainingSize() const { return (int)m_buffer.size() - m_position; }
	int GetTotalSize() const { return (int)m_buffer.size(); }

public:
	std::vector<uint8_t>& m_buffer;
	int m_position = 0;
	BufferEndian m_endianMode = BufferEndian::NATIVE;
};
