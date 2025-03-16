#include "Engine/Renderer/ConstantBuffer.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

ConstantBuffer::ConstantBuffer(size_t size)
	: m_size(size)
{
}

ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}