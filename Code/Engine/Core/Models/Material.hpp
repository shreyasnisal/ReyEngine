#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"

class Shader;
class Texture;

class Material
{
public:
	~Material() = default;
	Material() = default;
	explicit Material(Shader* shader, VertexType vertexType, Texture* diffuseTexture, Texture* normalTexture, Texture* specGlosEmitTexture);

public:
	Shader* m_shader = nullptr;
	VertexType m_vertexType = VertexType::VERTEX_PCUTBN;
	Texture* m_diffuseTexture = nullptr;
	Texture* m_normalTexture = nullptr;
	Texture* m_specGlosEmitTexture = nullptr;

public:
	static const Material DEFAULT;
};

inline const Material Material::DEFAULT = { };
