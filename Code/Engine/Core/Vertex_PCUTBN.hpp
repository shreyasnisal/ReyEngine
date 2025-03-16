#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

/*! \brief Position, Color, UV, Tangent, Bitangent, Normal Vertex
*
* A type of Vertex that contains the vertex position, color, UV texture coordinates, tangent, bitangent and normal.
*
*/
struct Vertex_PCUTBN
{
public:
	//! The position of the vertex
	Vec3 m_position;
	//! The vertex color
	Rgba8 m_color;
	//! The UV texture coordinates for the vertex
	Vec2 m_uvTexCoords;
	//! The tangent at this vertex
	Vec3 m_tangent;
	//! The bitangent at this vertex
	Vec3 m_bitangent;
	//! The normal at this vertex
	Vec3 m_normal;

public:
	//! Default destructor for Vertex_PCUTBN objects
	~Vertex_PCUTBN() = default;
	//! Default constructor for Vertex_PCUTBN objects
	Vertex_PCUTBN() = default;
	//! Default copy constructor for Vertex_PCUTBN objects
	Vertex_PCUTBN(Vertex_PCUTBN const& copyFrom) = default;
	explicit Vertex_PCUTBN(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords, Vec3 const& tangent, Vec3 const& bitangent, Vec3 const& normal);
};
