#include "Engine/Renderer/GPUMesh.hpp"

#include "Engine/Core/Models/CPUMesh.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

//! Destructor for GPUMesh- deletes VertexBuffer and IndexBuffer
GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;

	delete m_debugNormalsBuffer;
	m_debugNormalsBuffer = nullptr;
}

GPUMesh::GPUMesh(CPUMesh* cpuMesh, Renderer const* renderer)
	: m_cpuMesh(cpuMesh)
{
	m_vertexBuffer = renderer->CreateVertexBuffer(m_cpuMesh->m_vertexes.size() * sizeof(Vertex_PCUTBN), VertexType::VERTEX_PCUTBN);
	renderer->CopyCPUToGPU(m_cpuMesh->m_vertexes.data(), m_cpuMesh->m_vertexes.size() * sizeof(Vertex_PCUTBN), m_vertexBuffer);

	m_debugNormalsBuffer = renderer->CreateVertexBuffer(m_cpuMesh->m_debugNormalVertexes.size() * sizeof(Vertex_PCU), VertexType::VERTEX_PCU, true);
	renderer->CopyCPUToGPU(m_cpuMesh->m_debugNormalVertexes.data(), m_cpuMesh->m_debugNormalVertexes.size() * sizeof(Vertex_PCU), m_debugNormalsBuffer);

	if (m_cpuMesh->m_indexes.empty())
	{
		return;
	}

	m_indexBuffer = renderer->CreateIndexBuffer(m_cpuMesh->m_indexes.size() * sizeof(unsigned int));
	renderer->CopyCPUToGPU(m_cpuMesh->m_indexes.data(), m_cpuMesh->m_indexes.size() * sizeof(unsigned int), m_indexBuffer);\
}
