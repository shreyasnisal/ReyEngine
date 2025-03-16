#pragma once

#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"

#include <string>
#include <vector>

/*! \brief A list of vertexes and indexes corresponding to a 3D model
* 
* \sa GPUMesh
* 
*/
struct CPUMesh
{
public:
	//! The name of the CPUMesh
	std::string m_name;
	//! A list of vertexes for the CPUMesh
	std::vector<Vertex_PCUTBN> m_vertexes;
	//! A list of indexes for the GPUMesh
	std::vector<unsigned int> m_indexes;

	std::vector<Vertex_PCU> m_debugNormalVertexes;

public:
	~CPUMesh() = default;
	CPUMesh() = default;
	CPUMesh(CPUMesh const& copyFrom) = default;
	explicit CPUMesh(std::string const& name);
	explicit CPUMesh(std::string const& name, std::vector<Vertex_PCUTBN> const& vertexes);
	explicit CPUMesh(std::string const& name, std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indexes);

	void CalculateTangentBasis(bool calculateCrossProductNormals, bool calculateTangents);
	void CalculateTBN(Vertex_PCUTBN& vertex0, Vertex_PCUTBN& vertex1, Vertex_PCUTBN& vertex2, bool calculateCrossProductNormals, bool calculateTangents);
};

