#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Models/Material.hpp"
#include "Engine/Math/Mat44.hpp"

#include <vector>


struct CPUMesh;
struct GPUMesh;
class IndexBuffer;
class Renderer;
class Texture;
class VertexBuffer;

#include <string>

struct ModelGroup
{
	std::string m_name = "";
	CPUMesh* m_cpuMesh = nullptr;
	GPUMesh* m_gpuMesh = nullptr;

	ModelGroup() = default;
	ModelGroup(std::string name) : m_name(name) {};
};

/* \brief Stores the CPUMesh and GPUMesh for a 3D model
* 
* \sa CPUMesh
* \sa GPUMesh
* 
*/
struct Model
{
public:
	//! The name of the Model
	std::string m_name;
	std::vector<ModelGroup> m_groups;
	CPUMesh* m_cpuMesh = nullptr;
	GPUMesh* m_gpuMesh = nullptr;
	
public:
	~Model();
	Model(std::string name);
	Model(Model const& copyFrom) = default;
	Model(std::string name, std::vector<ModelGroup> const& groups, Renderer* renderer);

	VertexBuffer* GetVertexBuffer() const;
	VertexBuffer* GetVertexBuffer(char const* groupName) const;
	IndexBuffer* GetIndexBuffer() const;
	IndexBuffer* GetIndexBuffer(char const* groupName) const;
	int GetVertexCount() const;
	int GetVertexCount(char const* groupName) const;
	int GetIndexCount() const;
	int GetIndexCount(char const* groupName) const;
	VertexBuffer* GetDebugNormalsVertexBuffer() const;
	VertexBuffer* GetDebugNormalsVertexBuffer(char const* groupName) const;
	int GetDebugNormalsVertexCount() const;
	int GetDebugNormalsVertexCount(char const* groupName) const;
	int GetNumGroups() const;
	int GetGroupIndexFromName(char const* groupName) const;
};
