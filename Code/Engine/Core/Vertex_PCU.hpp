#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

/*! \brief Position, Color, UV Vertex
*
* A type of Vertex that contains the vertex position, color and UV texture coordinates.
* 
*/
struct Vertex_PCU
{
public:
	//! The position of the vertex
	Vec3 m_position;
	//! The color of the vertex
	Rgba8 m_color;
	//! The UV texture coordinates of the vertex
	Vec2 m_uvTexCoords;

public:
	//! Default destructor for a Vertex_PCU object
	~Vertex_PCU() = default;
	//! Default constructor for a Vertex_PCU object
	Vertex_PCU() = default;
	explicit Vertex_PCU(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords);
};
