#include "Engine/Renderer/Texture.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

Texture::~Texture()
{
	DX_SAFE_RELEASE(m_texture);
	DX_SAFE_RELEASE(m_shaderResourceView);
	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_depthStencilView);
}