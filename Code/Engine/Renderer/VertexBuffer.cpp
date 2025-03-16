#include "Engine/Renderer/VertexBuffer.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

VertexBuffer::VertexBuffer(size_t size)
	: m_size(size)
{
}

VertexBuffer::~VertexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

void VertexBuffer::SetIsLinePrimitive(bool isLinePrimitive)
{
	m_isLinePrimitive = isLinePrimitive;
}
