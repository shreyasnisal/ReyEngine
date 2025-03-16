#include "Engine/Core/Vertex_PCU.hpp"

/*! \brief Constructs a Vertex_PCU object from a position, color and UV coordinates
* 
* \param position A Vec3 representing the position of the vertex
* \param color An Rgba8 color representing the color of the vertex
* \param uvTextCoords A Vec2 representing the UV texture coordinates of the vertex
* 
*/
Vertex_PCU::Vertex_PCU(Vec3 const& position, Rgba8 const& color, Vec2 const& uvTexCoords)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
{

}
