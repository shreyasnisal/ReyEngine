#include "Engine/Core/BufferWriter.hpp"



BufferWriter::BufferWriter(std::vector<uint8_t>& buffer)
	: m_buffer(buffer)
{
	m_initialBufferSize = (int)buffer.size();
}

void BufferWriter::SetEndianMode(BufferEndian endianMode)
{
	m_endianMode = endianMode;
	if (m_endianMode != GetPlatformNativeEndianMode())
	{
		m_isWritingInOppositeEndianMode = true;
	}
	else
	{
		m_isWritingInOppositeEndianMode = false;
	}
}

void BufferWriter::AppendChar(unsigned char charToAppend)
{
	m_buffer.push_back(charToAppend);
}

void BufferWriter::AppendByte(unsigned char byteToAppend)
{
	m_buffer.push_back(byteToAppend);
}

void BufferWriter::AppendBool(bool boolToAppend)
{
	m_buffer.push_back(boolToAppend);
}

void BufferWriter::AppendUint32(uint32_t uint32ToAppend)
{
	uint8_t* uint32Bytes = reinterpret_cast<uint8_t*>(&uint32ToAppend);
	m_buffer.push_back(uint32Bytes[0]);
	m_buffer.push_back(uint32Bytes[1]);
	m_buffer.push_back(uint32Bytes[2]);
	m_buffer.push_back(uint32Bytes[3]);
}

void BufferWriter::AppendInt32(int32_t int32ToAppend)
{
	uint8_t* int32Bytes = reinterpret_cast<uint8_t*>(&int32ToAppend);
	m_buffer.push_back(int32Bytes[0]);
	m_buffer.push_back(int32Bytes[1]);
	m_buffer.push_back(int32Bytes[2]);
	m_buffer.push_back(int32Bytes[3]);
}

void BufferWriter::AppendFloat(float floatToAppend)
{
	uint8_t* floatBytes = reinterpret_cast<uint8_t*>(&floatToAppend);
	m_buffer.push_back(floatBytes[0]);
	m_buffer.push_back(floatBytes[1]);
	m_buffer.push_back(floatBytes[2]);
	m_buffer.push_back(floatBytes[3]);
}

void BufferWriter::AppendDouble(double doubleToAppend)
{
	uint8_t* doubleBytes = reinterpret_cast<uint8_t*>(&doubleToAppend);
	m_buffer.push_back(doubleBytes[0]);
	m_buffer.push_back(doubleBytes[1]);
	m_buffer.push_back(doubleBytes[2]);
	m_buffer.push_back(doubleBytes[3]);
	m_buffer.push_back(doubleBytes[4]);
	m_buffer.push_back(doubleBytes[5]);
	m_buffer.push_back(doubleBytes[6]);
	m_buffer.push_back(doubleBytes[7]);
}

void BufferWriter::AppendStringZeroTerminated(std::string stringToAppend)
{
	for (int charIndex = 0; charIndex < (int)stringToAppend.length(); charIndex++)
	{
		AppendChar(stringToAppend[charIndex]);
	}
	AppendChar(0);
}

void BufferWriter::AppendStringAfter32BitLength(std::string stringToAppend)
{
	AppendUint32((uint32_t)stringToAppend.length());
	for (int charIndex = 0; charIndex < (int)stringToAppend.length(); charIndex++)
	{
		AppendChar(stringToAppend[charIndex]);
	}
}

void BufferWriter::AppendRgba(Rgba8 const& rgbaToAppend)
{
	AppendByte(rgbaToAppend.r);
	AppendByte(rgbaToAppend.g);
	AppendByte(rgbaToAppend.b);
	AppendByte(rgbaToAppend.a);
}

void BufferWriter::AppendRgb(Rgba8 const& rgbToAppend)
{
	AppendByte(rgbToAppend.r);
	AppendByte(rgbToAppend.g);
	AppendByte(rgbToAppend.b);
}

void BufferWriter::AppendIntVec2(IntVec2 const& intVec2ToAppend)
{
	AppendInt32(intVec2ToAppend.x);
	AppendInt32(intVec2ToAppend.y);
}

void BufferWriter::AppendVec2(Vec2 const& vec2ToAppend)
{
	AppendFloat(vec2ToAppend.x);
	AppendFloat(vec2ToAppend.y);
}

void BufferWriter::AppendVec3(Vec3 const& vec3ToAppend)
{
	AppendFloat(vec3ToAppend.x);
	AppendFloat(vec3ToAppend.y);
	AppendFloat(vec3ToAppend.z);
}

void BufferWriter::AppendEulerAngles(EulerAngles const& eulerAnglesToAppend)
{
	AppendFloat(eulerAnglesToAppend.m_yawDegrees);
	AppendFloat(eulerAnglesToAppend.m_pitchDegrees);
	AppendFloat(eulerAnglesToAppend.m_rollDegrees);
}

void BufferWriter::AppendVertexPCU(Vertex_PCU const& vertexPCUToAppend)
{
	AppendVec3(vertexPCUToAppend.m_position);
	AppendRgba(vertexPCUToAppend.m_color);
	AppendVec2(vertexPCUToAppend.m_uvTexCoords);
}
