#include "Engine/Core/BufferParser.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"


BufferParser::BufferParser(std::vector<uint8_t>& buffer)
	: m_buffer(buffer)
	, m_position(0)
{
}

void BufferParser::SetEndianMode(BufferEndian endianMode)
{
	m_endianMode = endianMode;
	if (m_endianMode != GetPlatformNativeEndianMode())
	{
		m_isReadingInOppositeEndianMode = true;
	}
	else
	{
		m_isReadingInOppositeEndianMode = false;
	}
}

unsigned char BufferParser::ParseChar()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + 1, "Buffer position out of bounds for parsing char");

	return m_buffer[m_position++];
}

unsigned char BufferParser::ParseByte()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + 1, "Buffer position out of bounds for parsing byte");

	return m_buffer[m_position++];
}

bool BufferParser::ParseBool()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + 1, "Buffer position out of bounds for parsing bool");

	return m_buffer[m_position++];
}

short BufferParser::ParseShort()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(short), "Buffer position out of bounds for parsing short");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(short);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseWordBytesInPlace(bytes);
	}

	short value = *reinterpret_cast<short*>(bytes);
	return value;
}

unsigned short BufferParser::ParseUShort()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(unsigned short), "Buffer position out of bounds for parsing ushort");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(short);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseWordBytesInPlace(bytes);
	}

	unsigned short value = *reinterpret_cast<short*>(bytes);
	return value;
}

uint32_t BufferParser::ParseUint32()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(uint32_t), "Buffer position out of bounds for parsing uint32");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(uint32_t);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseWordBytesInPlace(bytes);
	}

	uint32_t value = *reinterpret_cast<uint32_t*>(bytes);
	return value;
}

int32_t BufferParser::ParseInt32()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(int32_t), "Buffer position out of bounds for parsing int32");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(int32_t);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseWordBytesInPlace(bytes);
	}

	int32_t value = *reinterpret_cast<int32_t*>(bytes);
	return value;
}

uint64_t BufferParser::ParseUint64()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(uint64_t), "Buffer position out of bounds for parsing uint64");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(uint64_t);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseWordBytesInPlace(bytes);
	}

	uint64_t value = *reinterpret_cast<short*>(bytes);
	return value;
}

int64_t BufferParser::ParseInt64()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(int64_t), "Buffer position out of bounds for parsing int64");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(int64_t);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseWordBytesInPlace(bytes);
	}

	int64_t value = *reinterpret_cast<short*>(bytes);
	return value;
}

float BufferParser::ParseFloat()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(float), "Buffer position out of bounds for parsing float");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(float);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseWordBytesInPlace(bytes);
	}

	float value = *reinterpret_cast<float*>(bytes);
	return value;
}

double BufferParser::ParseDouble()
{
	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + sizeof(double), "Buffer position out of bounds for parsing double");

	uint8_t* bytes = &m_buffer[m_position];
	m_position += sizeof(double);

	if (m_isReadingInOppositeEndianMode)
	{
		ReverseDWordBytesInPlace(bytes);
	}

	double value = *reinterpret_cast<double*>(bytes);
	return value;
}

void BufferParser::ParseStringZeroTerminated(std::string& out_string)
{
	char strChar = ParseChar();
	while (strChar != 0 && m_position < m_buffer.size())
	{
		out_string += strChar;
		strChar = ParseChar();
	}
}

void BufferParser::ParseStringAfter32BitLength(std::string& out_string)
{
	uint32_t strLength = ParseUint32();

	GUARANTEE_OR_DIE(m_buffer.size() >= m_position + strLength, "Buffer position out of bounds for parsing string");

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

void BufferParser::SetSeekPosition(int seekPosition)
{
	m_position = seekPosition;
}
