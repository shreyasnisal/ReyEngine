#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#include <string>
#include <vector>


class BufferWriter
{
public:
	BufferWriter(std::vector<uint8_t>& buffer);

	void SetEndianMode(BufferEndian endianMode);
	BufferEndian GetEndianMode() const { return m_endianMode; }

	void AppendChar(unsigned char charToAppend);
	void AppendByte(unsigned char byteToAppend);
	void AppendBool(bool boolToAppend);
	void AppendUint32(uint32_t uint32ToAppend);
	void AppendInt32(int32_t int32ToAppend);
	void AppendFloat(float floatToAppend);
	void AppendDouble(double doubleToAppend);
	void AppendStringZeroTerminated(std::string stringToAppend);
	void AppendStringAfter32BitLength(std::string stringToAppend);

	void AppendRgba(Rgba8 const& rgbaToAppend);
	void AppendRgb(Rgba8 const& rgbToAppend);
	void AppendIntVec2(IntVec2 const& intVec2ToAppend);
	void AppendVec2(Vec2 const& vec2ToAppend);
	void AppendVec3(Vec3 const& vec3ToAppend);
	void AppendEulerAngles(EulerAngles const& eulerAnglesToAppend);
	void AppendVertexPCU(Vertex_PCU const& vertexPCUToAppend);

	int GetAppendedSize() { return (int)m_buffer.size() - m_initialBufferSize; }
	int GetTotalSize() { return (int)m_buffer.size(); }

public:
	std::vector<uint8_t>& m_buffer;
	int m_initialBufferSize = 0;
	BufferEndian m_endianMode = BufferEndian::NATIVE;
	bool m_isWritingInOppositeEndianMode = false;
};
