#include "Engine/Core/Models/CPUMesh.hpp"

#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"

CPUMesh::CPUMesh(std::string const& name)
	: m_name(name)
{
}

CPUMesh::CPUMesh(std::string const& name, std::vector<Vertex_PCUTBN> const& vertexes)
	: m_name(name)
	, m_vertexes(vertexes)
{
	for (int vertexIndex = 0; vertexIndex < (int)m_vertexes.size(); vertexIndex++)
	{
		m_indexes.push_back((unsigned int)vertexIndex);
	}
}

CPUMesh::CPUMesh(std::string const& name, std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indexes)
	: m_name(name)
	, m_vertexes(vertexes)
	, m_indexes(indexes)
{
}

void CPUMesh::CalculateTangentBasis(bool calculateCrossProductNormals, bool calculateTangents)
{
	for (int index = 0; index < (int)m_indexes.size(); index += 3)
	{
		CalculateTBN(m_vertexes[m_indexes[index]], m_vertexes[m_indexes[index + 1]], m_vertexes[m_indexes[index + 2]], calculateCrossProductNormals, calculateTangents);
	}

	for (int vertexIndex = 0; vertexIndex < (int)m_vertexes.size(); vertexIndex++)
	{		
		// Gram Schmidt
		Mat44 matrixForGramSchmidt(m_vertexes[vertexIndex].m_tangent, m_vertexes[vertexIndex].m_bitangent, m_vertexes[vertexIndex].m_normal, Vec3::ZERO);
		matrixForGramSchmidt.Orthonormalize_iFwd_jLeft_kUp_PreserveK();

		m_vertexes[vertexIndex].m_tangent = matrixForGramSchmidt.GetIBasis3D();
		m_vertexes[vertexIndex].m_bitangent = matrixForGramSchmidt.GetJBasis3D();
		m_vertexes[vertexIndex].m_normal = matrixForGramSchmidt.GetKBasis3D();
	}


	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Create DebugNormals vertexes
	for (int vertexIndex = 0; vertexIndex < (int)m_vertexes.size(); vertexIndex++)
	{
		Vertex_PCU debugTangentVertex1(m_vertexes[vertexIndex].m_position, Rgba8::RED, Vec2::ZERO);
		m_debugNormalVertexes.push_back(debugTangentVertex1);
		Vertex_PCU debugTangentVertex2(m_vertexes[vertexIndex].m_position + m_vertexes[vertexIndex].m_tangent * 0.1f, Rgba8::RED, Vec2::ZERO);
		m_debugNormalVertexes.push_back(debugTangentVertex2);

		Vertex_PCU debugBitangentVertex1(m_vertexes[vertexIndex].m_position, Rgba8::GREEN, Vec2::ZERO);
		m_debugNormalVertexes.push_back(debugBitangentVertex1);
		Vertex_PCU debugBitangentVertex2(m_vertexes[vertexIndex].m_position + m_vertexes[vertexIndex].m_bitangent * 0.1f, Rgba8::GREEN, Vec2::ZERO);
		m_debugNormalVertexes.push_back(debugBitangentVertex2);

		Vertex_PCU debugNormalVertex1(m_vertexes[vertexIndex].m_position, Rgba8::BLUE, Vec2::ZERO);
		m_debugNormalVertexes.push_back(debugNormalVertex1);
		Vertex_PCU debugNormalVertex2(m_vertexes[vertexIndex].m_position + m_vertexes[vertexIndex].m_normal * 0.1f, Rgba8::BLUE, Vec2::ZERO);
		m_debugNormalVertexes.push_back(debugNormalVertex2);
	}
}

void CPUMesh::CalculateTBN(Vertex_PCUTBN& vertex0, Vertex_PCUTBN& vertex1, Vertex_PCUTBN& vertex2, bool calculateCrossProductNormals, bool calculateTangents)
{
	Vec3 vertex0To1 = vertex1.m_position - vertex0.m_position;
	Vec3 vertex0To2 = vertex2.m_position - vertex0.m_position;

	if (calculateCrossProductNormals)
	{
		Vec3 normal = (CrossProduct3D(vertex0To1, vertex0To2) / DotProduct3D(vertex0To1, vertex0To2) * DotProduct3D(vertex0To1, vertex0To2));
		vertex0.m_normal += normal;
		vertex1.m_normal += normal;
		vertex2.m_normal += normal;
	}

	if (calculateTangents)
	{
		Vec3 deltaUV0To1 = (vertex1.m_uvTexCoords - vertex0.m_uvTexCoords).ToVec3();
		Vec3 deltaUV0To2 = (vertex2.m_uvTexCoords - vertex0.m_uvTexCoords).ToVec3();

		float r = 1.f / (deltaUV0To1.x * deltaUV0To2.y - deltaUV0To2.x * deltaUV0To1.y);
		Vec3 tangent = (r * (deltaUV0To2.y * vertex0To1 - deltaUV0To1.y * vertex0To2)).GetNormalized();
		vertex0.m_tangent += tangent;
		vertex1.m_tangent += tangent;
		vertex2.m_tangent += tangent;

		Vec3 bitangent = (r * (deltaUV0To1.x * vertex0To2 - deltaUV0To2.x * vertex0To1)).GetNormalized();
		vertex0.m_bitangent += bitangent;
		vertex1.m_bitangent += bitangent;
		vertex2.m_bitangent += bitangent;
	}
}
