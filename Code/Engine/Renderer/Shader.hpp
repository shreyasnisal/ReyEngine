#pragma once

#include <string>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

struct ShaderConfig
{
public:
	std::string m_name;
	std::string m_vertexEntryPoint = "VertexMain";
	std::string m_pixelEntryPoint = "PixelMain";
};

class Shader
{
	friend class Renderer;

protected:
	~Shader();
	Shader(ShaderConfig const& config);
	Shader(Shader const& copy) = delete;

	std::string const& GetName() const;

protected:
	ShaderConfig				m_config;
	ID3D11VertexShader*			m_vertexShader = nullptr;
	ID3D11PixelShader*			m_pixelShader = nullptr;
	ID3D11InputLayout*			m_inputLayout = nullptr;
};