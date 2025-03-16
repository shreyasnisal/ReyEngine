#pragma once

#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <string>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct SurfaceData;

class Image;

class Texture
{
	friend class Renderer;
	friend struct SurfaceData;

private:
	~Texture();
	Texture() = default;
	Texture(Texture const& copy) = delete;

public:
	IntVec2							GetDimensions() const		{ return m_dimensions; }
	std::string const&				GetImageFilePath() const	{ return m_name; }

protected:
	std::string						m_name;
	IntVec2							m_dimensions;

	ID3D11Texture2D*				m_texture = nullptr;
	ID3D11ShaderResourceView*		m_shaderResourceView = nullptr;
	ID3D11RenderTargetView*			m_renderTargetView = nullptr;
	ID3D11DepthStencilView*			m_depthStencilView = nullptr;
};