#include "Engine/Core/Models/Model.hpp"

#include "Engine/Core/Models/CPUMesh.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

Model::~Model()
{
	for (int groupIndex = 0; groupIndex < (int)m_groups.size(); groupIndex++)
	{
		delete m_groups[groupIndex].m_cpuMesh;
		m_groups[groupIndex].m_cpuMesh = nullptr;
		delete m_groups[groupIndex].m_gpuMesh;
		m_groups[groupIndex].m_gpuMesh = nullptr;
	}
	delete m_cpuMesh;
	m_cpuMesh = nullptr;
	delete m_gpuMesh;
	m_gpuMesh = nullptr;
}

Model::Model(std::string name)
	: m_name(name)
{
}

Model::Model(std::string name, std::vector<ModelGroup> const& groups, Renderer* renderer)
	: m_name(name)
	, m_groups(groups)
{
	std::vector<Vertex_PCUTBN> allVertexes;
	std::vector<unsigned int> allIndexes;
	for (int groupIndex = 0; groupIndex < (int)groups.size(); groupIndex++)
	{
		std::vector<Vertex_PCUTBN> const& groupVertexes = groups[groupIndex].m_cpuMesh->m_vertexes;
		std::vector<unsigned int> const& groupIndexes = groups[groupIndex].m_cpuMesh->m_indexes;

		unsigned int startIndex = (unsigned int)allVertexes.size();

		for (int vertexIndex = 0; vertexIndex < (int)groupVertexes.size(); vertexIndex++)
		{
			allVertexes.push_back(groupVertexes[vertexIndex]);
		}

		for (int index = 0; index < (int)groupIndexes.size(); index++)
		{
			allIndexes.push_back(startIndex + groupIndexes[index]);
		}
	}

	m_cpuMesh = new CPUMesh(m_name, allVertexes, allIndexes);
	m_cpuMesh->CalculateTangentBasis(false, true);
	m_gpuMesh = new GPUMesh(m_cpuMesh, renderer);
}

VertexBuffer* Model::GetVertexBuffer() const
{
	return m_gpuMesh->m_vertexBuffer;
}

VertexBuffer* Model::GetVertexBuffer(char const* groupName) const
{
	int groupIndex = GetGroupIndexFromName(groupName);
	if (groupIndex != -1)
	{
		return m_groups[groupIndex].m_gpuMesh->m_vertexBuffer;
	}

	return nullptr;
}

IndexBuffer* Model::GetIndexBuffer() const
{
	return m_gpuMesh->m_indexBuffer;
}

IndexBuffer* Model::GetIndexBuffer(char const* groupName) const
{
	int groupIndex = GetGroupIndexFromName(groupName);
	if (groupIndex != -1)
	{
		return m_groups[groupIndex].m_gpuMesh->m_indexBuffer;
	}

	return nullptr;
}

int Model::GetVertexCount() const
{
	return (int)m_cpuMesh->m_vertexes.size();
}

int Model::GetVertexCount(char const* groupName) const
{
	int groupIndex = GetGroupIndexFromName(groupName);
	if (groupIndex != -1)
	{
		return (int)(m_groups[groupIndex].m_cpuMesh->m_vertexes.size());
	}

	return 0;
}

int Model::GetIndexCount() const
{
	return (int)m_cpuMesh->m_indexes.size();
}

int Model::GetIndexCount(char const* groupName) const
{
	int groupIndex = GetGroupIndexFromName(groupName);
	if (groupIndex != -1)
	{
		return (int)(m_groups[groupIndex].m_cpuMesh->m_indexes.size());
	}

	return 0;
}

VertexBuffer* Model::GetDebugNormalsVertexBuffer() const
{
	return m_gpuMesh->m_debugNormalsBuffer;
}

VertexBuffer* Model::GetDebugNormalsVertexBuffer(char const* groupName) const
{
	int groupIndex = GetGroupIndexFromName(groupName);
	if (groupIndex != -1)
	{
		return m_groups[groupIndex].m_gpuMesh->m_debugNormalsBuffer;
	}

	return nullptr;
}

int Model::GetDebugNormalsVertexCount() const
{
	return (int)m_cpuMesh->m_debugNormalVertexes.size();
}

int Model::GetDebugNormalsVertexCount(char const* groupName) const
{
	int groupIndex = GetGroupIndexFromName(groupName);
	if (groupIndex != -1)
	{
		return (int)(m_groups[groupIndex].m_cpuMesh->m_debugNormalVertexes.size());
	}

	return 0;
}

int Model::GetNumGroups() const
{
	return (int)m_groups.size();
}

int Model::GetGroupIndexFromName(char const* groupName) const
{
	for (int groupIndex = 0; groupIndex < (int)m_groups.size(); groupIndex++)
	{
		if (!strcmp(m_groups[groupIndex].m_name.c_str(), groupName))
		{
			return groupIndex;
		}
	}

	return -1;
}
