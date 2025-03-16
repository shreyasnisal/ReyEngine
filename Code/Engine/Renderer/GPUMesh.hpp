#pragma once

struct CPUMesh;
class VertexBuffer;
class IndexBuffer;
class Renderer;

/*! \brief A list of vertexes and indexes corresponding to a 3D model
*
* \sa GPUMesh
*/
struct GPUMesh
{
public:
	CPUMesh* m_cpuMesh = nullptr;
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	VertexBuffer* m_debugNormalsBuffer = nullptr;

public:
	~GPUMesh();
	GPUMesh() = default;
	GPUMesh(GPUMesh const& copyFrom) = delete;
	GPUMesh(CPUMesh* cpuMesh, Renderer const* renderer);
};

