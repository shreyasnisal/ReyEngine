#include "Engine/Core/Models/Material.hpp"

Material::Material(Shader* shader, VertexType vertexType, Texture* diffuseTexture, Texture* normalTexture, Texture* specGlosEmitTexture)
	: m_shader(shader)
	, m_vertexType(vertexType)
	, m_diffuseTexture(diffuseTexture)
	, m_normalTexture(normalTexture)
	, m_specGlosEmitTexture(specGlosEmitTexture)
{
}
