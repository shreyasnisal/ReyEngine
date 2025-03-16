#include "Engine/Core/BufferParser.hpp"

BufferParser::BufferParser(std::vector<uint8_t>& buffer)
	: m_buffer(buffer)
	, m_position(0)
{
}

void BufferParser::SetEndianMode(BufferEndian endianMode)
{
	m_endianMode = endianMode;
}

unsigned char BufferParser::ParseChar()
{
	return m_buffer[m_position++];
}

unsigned char BufferParser::ParseByte()
{
	return m_buffer[m_position++];
}

bool BufferParser::ParseBool()
{
	return m_buffer[m_position++];
}

uint32_t BufferParser::ParseUint32()
{
	uint8_t* bytes = &m_buffer[m_position];
	m_position += 4;
	uint32_t value = *reinterpret_cast<uint32_t*>(bytes);
	return value;
}

int32_t BufferParser::ParseInt32()
{
	uint8_t* bytes = &m_buffer[m_position];
	m_position += 4;
	int32_t value = *reinterpret_cast<int32_t*>(bytes);
	return value;
}

float BufferParser::ParseFloat()
{
	uint8_t* bytes = &m_buffer[m_position];
	m_position += 4;
	float value = *reinterpret_cast<float*>(bytes);
	return value;
}

double BufferParser::ParseDouble()
{
	uint8_t* bytes = &m_buffer[m_position];
	m_position += 8;
	double value = *reinterpret_cast<double*>(bytes);
	return value;
}

void BufferParser::ParseStringZeroTerminated(std::string& out_string)
{
	char strChar = ParseChar();
	while (strChar != 0)
	{
		out_string += strChar;
		strChar = ParseChar();
	}
}

void BufferParser::ParseStringAfter32BitLength(std::string& out_string)
{
	uint32_t strLength = ParseUint32();
	for (int charIndex = 0; charIndex < (int)strLength; charIndex++)
	{
		out_string += ParseChar();
	}
}

Rgba8 const BufferParser::ParseRgba()
{
	Rgba8 result;
	result.r = ParseByte();
	result.g = ParseByte();
	result.b = ParseByte();
	result.a = ParseByte();
	return result;
}

Rgba8 const BufferParser::ParseRgb()
{
	Rgba8 result;
	result.r = ParseByte();
	result.g = ParseByte();
	result.b = ParseByte();
	result.a = 255;
	return result;
}

IntVec2 const BufferParser::ParseIntVec2()
{
	IntVec2 result;
	result.x = ParseInt32();
	result.y = ParseInt32();
	return result;
}

Vec2 const BufferParser::ParseVec2()
{
	Vec2 result;
	result.x = ParseFloat();
	result.y = ParseFloat();
	return result;
}

Vec3 const BufferParser::ParseVec3()
{
	Vec3 result;
	result.x = ParseFloat();
	result.y = ParseFloat();
	result.z = ParseFloat();
	return result;
}

EulerAngles const BufferParser::ParseEulerAngles()
{
	EulerAngles result;
	result.m_yawDegrees = ParseFloat();
	result.m_pitchDegrees = ParseFloat();
	result.m_rollDegrees = ParseFloat();
	return result;
}

Vertex_PCU const BufferParser::ParseVertexPCU()
{
	Vertex_PCU result;
	result.m_position = ParseVec3();
	result.m_color = ParseRgba();
	result.m_uvTexCoords = ParseVec2();
	return result;
}
