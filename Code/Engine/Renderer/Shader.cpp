#include "Engine/Renderer/Shader.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

Shader::Shader(ShaderConfig const& config)
	: m_config(config)
{
}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_vertexShader);
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_inputLayout);
}

std::string const& Shader::GetName() const
{
	return m_config.m_name;
}