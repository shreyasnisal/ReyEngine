#include "VertexUtils.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Vec2.hpp"

#include <math.h>

/*! \brief Transforms all vertexes in 2D (using X and Y values) for a given array of Vertex_PCU objects
* 
* This function uses a uniform scale, rotation about the Z-axis and a 2D translation to transform the vertex array. It converts the scale, rotation and translation (in that order) to basis vectors and then transforms each vertex using these vectors. The function is useful for transforming vertexes from local space to world space.
* \param numVerts An integer representing the total number of vertexes to transform starting from the address passed in
* \param verts A Vertex_PCU pointer representing the starting address of the vertexes to transform
* \param uniformScaleXY A floating point number representing the scaling factor to apply to the vertexes
* \param rotationDegreesAboutZ A floating point number representing the rotation to apply to the vertexes
* \param translationXY A Vec2 representing the 2D translation to apply to the vertexes
* 
*/
void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Vec2 iBasis(1.f, 0.f);
	Vec2 jBasis(0.f, 1.f);

	iBasis *= uniformScaleXY;
	jBasis *= uniformScaleXY;

	iBasis.RotateDegrees(rotationDegreesAboutZ);
	jBasis.RotateDegrees(rotationDegreesAboutZ);

	for (int vertexIndex = 0; vertexIndex < numVerts; vertexIndex++)
	{
		TransformPositionXY3D(verts[vertexIndex].m_position, iBasis, jBasis, translationXY);
	}
}

/*! \brief Transforms all vertexes in 2D (using X and Y values) for a given array of Vertex_PCU objects
* 
* This function performs the same operation (and internally calls) the TransformVertexArray(int, Vertex_PCU*, float, float, const Vec2&).
* \param verts A list of Vertex_PCU objects, passed by reference, representing the vertexes to transform
* \param uniformScaleXY A floating point number representing the scaling factor to apply to the vertexes
* \param rotationDegreesAboutZ A floating point number representing the rotation to apply to the vertexes
* \param translationXY A Vec2 representing the 2D translation to apply to the vertexes
* \sa <a href="_vertex_utils_8hpp.html#aca2c279113f21c4732f68196d690840c">TransformVertexArray(int, Vertex_PCU*, float, float, const Vec2&)</a>
* 
*/
void TransformVertexArrayXY3D(std::vector<Vertex_PCU>& verts, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	TransformVertexArrayXY3D(static_cast<int>(verts.size()), verts.data(), uniformScaleXY, rotationDegreesAboutZ, translationXY);
}

/*! \brief Transforms all vertexes in 3D for a given array of Vertex_PCU objects
* 
* Uses the transformation matrix provided to transform the list of vertexes (simply multiplying each vertex (as a position) with the matrix).
* \param verts A list of Vertex_PCU objects, passed by reference, representing the vertexes to transform
* \param transform A Mat44 matrix representing the transformation to apply to the vertexes
* 
*/
void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, Mat44 const& transform)
{
	for (int vertexIndex = 0; vertexIndex < static_cast<int>(verts.size()); vertexIndex++)
	{
		verts[vertexIndex].m_position = transform.TransformPosition3D(verts[vertexIndex].m_position);
	}
}

/*! \brief Transforms all vertexes in 3D for a given array of Vertex_PCUTBN objects
*
* Uses the transformation matrix provided to transform the list of vertexes (simply multiplying each vertex (as a position) with the matrix).
* \param verts A list of Vertex_PCU objects, passed by reference, representing the vertexes to transform
* \param transform A Mat44 matrix representing the transformation to apply to the vertexes
*
*/
void TransformVertexArray3D(std::vector<Vertex_PCUTBN>& verts, Mat44 const& transform)
{
	for (int vertexIndex = 0; vertexIndex < static_cast<int>(verts.size()); vertexIndex++)
	{
		verts[vertexIndex].m_position = transform.TransformPosition3D(verts[vertexIndex].m_position);
	}
}

/*! \brief Gets the bounds that the vertexes in a list are contained in
* 
* \param verts A list of vertexes for which the bounds should be calculated
* \return An AABB2 representing the bounds enclosing all vertexes in the array
* 
*/
AABB2 GetVertexBounds2D(std::vector<Vertex_PCU> const& verts)
{
	AABB2 bounds(Vec2(FLT_MAX, FLT_MAX), Vec2(FLT_MIN, FLT_MIN));
	for (int vertexIndex = 0; vertexIndex < static_cast<int>(verts.size()); vertexIndex++)
	{
		Vec2 const& vertexPosXY = verts[vertexIndex].m_position.GetXY();
		if (vertexPosXY.x < bounds.m_mins.x)
		{
			bounds.m_mins.x = vertexPosXY.x;
		}
		if (vertexPosXY.x > bounds.m_maxs.x)
		{
			bounds.m_maxs.x = vertexPosXY.x;
		}
		if (vertexPosXY.y < bounds.m_mins.y)
		{
			bounds.m_mins.y = vertexPosXY.y;
		}
		if (vertexPosXY.y > bounds.m_maxs.y)
		{
			bounds.m_maxs.y = vertexPosXY.y;
		}
	}

	return bounds;
}

/*! \brief Adds vertexes for rendering a capsule to a list of vertexes
* 
* Pushes back the vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the capsule.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for a capsule should be appended
* \param boneStart A Vec2 representing the starting point for the "bone" of the capsule
* \param boneEnd A Vec2 representing the ending point for the "bone" of the capsule
* \param radius The radius of the capsule
* \param color The Rgba8 color that the capsule should be drawn in
* 
*/
void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color)
{
	float capsuleLength = GetDistance2D(boneStart, boneEnd);
	Vec2 capsuleDirection = (boneEnd - boneStart).GetNormalized();

	OBB2 capsuleBox = OBB2(boneStart + capsuleDirection * capsuleLength * 0.5f, capsuleDirection, Vec2(capsuleLength * 0.5f, radius));
	
	AddVertsForDirectedSector2D(verts, boneStart, -capsuleDirection, 180.f, radius, color);
	AddVertsForOBB2(verts, capsuleBox, color);
	AddVertsForDirectedSector2D(verts, boneEnd, capsuleDirection, 180.f, radius, color);
}

/*! \brief Adds vertexes for rendering a disc to a list of vertexes
* 
* Pushes back the vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function supports textures on the disc.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for a disc should be appended
* \param center A Vec2 representing the center of the disc
* \param radius The radius of the disc
* \param color The Rgba8 color that the disc should be drawn in
* \param uvAtMins A Vec2 representing the texture UV coordinate minimum values
* \param uvAtMaxs A Vec2 representing the texture UV coordinate maximum values
* 
*/
void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs, int numTris)
{
	UNUSED(uvAtMaxs);
	//constexpr int NUM_TRIS = 16;
	const int numVerts = numTris * 3;
	const float DEGREES_INCREMENT_PER_VERTEX = 360.f / static_cast<float>(numVerts);

	Vec2 previousVertexPosition = center + Vec2::MakeFromPolarDegrees(0.f, radius);
	Vec2 previousVertexUVs = uvAtMins + Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(0.f, 0.5f);

	for (int vertexIndex = 0; vertexIndex < numVerts; vertexIndex += 3)
	{
		Vec2 newVertexPosition = center + Vec2::MakeFromPolarDegrees(static_cast<float>(vertexIndex + 3) * DEGREES_INCREMENT_PER_VERTEX, radius);
		Vec2 newVertexUVs = uvAtMins + Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(static_cast<float>(vertexIndex + 3) * DEGREES_INCREMENT_PER_VERTEX, 0.5f);
		Vertex_PCU centerVertex = Vertex_PCU(center.ToVec3(), color, uvAtMins + Vec2(0.5f, 0.5f));
		Vertex_PCU previousVertex = Vertex_PCU(previousVertexPosition.ToVec3(), color, previousVertexUVs);
		Vertex_PCU newVertex = Vertex_PCU(newVertexPosition.ToVec3(), color, newVertexUVs);

		verts.push_back(centerVertex);
		verts.push_back(previousVertex);
		verts.push_back(newVertex);

		previousVertexPosition = newVertexPosition;
		previousVertexUVs = newVertexUVs;
	}
}

void AddVertsForRing2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float thickness, Rgba8 const& color)
{
	constexpr int RING_TRAPEZOIDS = 64;
	constexpr float THETA_INCREMENT_DEGREES = 360.f / (float)(RING_TRAPEZOIDS);
	float innerRadius = radius - thickness * 0.5f;
	float outerRadius = radius + thickness * 0.5f;

	for (int trapIndex = 0; trapIndex < RING_TRAPEZOIDS; trapIndex++)
	{
		float startThetaDegrees = (float)(trapIndex)*THETA_INCREMENT_DEGREES;
		float endThetaDegrees = (float)(trapIndex + 1) * THETA_INCREMENT_DEGREES;

		Vec2 innerStartVertexPos = center + Vec2::MakeFromPolarDegrees(startThetaDegrees, innerRadius);
		Vec2 outerStartVertexPos = center + Vec2::MakeFromPolarDegrees(startThetaDegrees, outerRadius);
		Vec2 innerEndVertexPos = center + Vec2::MakeFromPolarDegrees(endThetaDegrees, innerRadius);
		Vec2 outerEndVertexPos = center + Vec2::MakeFromPolarDegrees(endThetaDegrees, outerRadius);

		verts.push_back(Vertex_PCU(Vec3(innerStartVertexPos.x, innerStartVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerStartVertexPos.x, outerStartVertexPos.y, 0.f),	color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(innerEndVertexPos.x, innerEndVertexPos.y, 0.f),	color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerStartVertexPos.x, outerStartVertexPos.y, 0.f),	color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerEndVertexPos.x, outerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(innerEndVertexPos.x, innerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
	}
}

void AddVertsForArc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float thickness, float startAngle, float endAngle, Rgba8 const& color)
{
	constexpr int RING_TRAPEZOIDS = 64;
	float thetaIncrementDegrees = (endAngle - startAngle) / (float)(RING_TRAPEZOIDS);
	float innerRadius = radius - thickness * 0.5f;
	float outerRadius = radius + thickness * 0.5f;

	for (int trapIndex = 0; trapIndex < RING_TRAPEZOIDS; trapIndex++)
	{
		float startThetaDegrees = startAngle + (float)(trapIndex) * thetaIncrementDegrees;
		float endThetaDegrees = startAngle + (float)(trapIndex + 1) * thetaIncrementDegrees;

		Vec2 innerStartVertexPos = center + Vec2::MakeFromPolarDegrees(startThetaDegrees, innerRadius);
		Vec2 outerStartVertexPos = center + Vec2::MakeFromPolarDegrees(startThetaDegrees, outerRadius);
		Vec2 innerEndVertexPos = center + Vec2::MakeFromPolarDegrees(endThetaDegrees, innerRadius);
		Vec2 outerEndVertexPos = center + Vec2::MakeFromPolarDegrees(endThetaDegrees, outerRadius);

		verts.push_back(Vertex_PCU(Vec3(innerStartVertexPos.x, innerStartVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerStartVertexPos.x, outerStartVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(innerEndVertexPos.x, innerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerStartVertexPos.x, outerStartVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerEndVertexPos.x, outerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(innerEndVertexPos.x, innerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
	}
}

/*! \brief Adds vertexes for rendering a sector to a list of vertexes
* 
* Pushes back the vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the sector. This function draws an "oriented" sector, where the orientation of the sector from the X-axis is given.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for a sector should be appended
* \param sectorTip A Vec2 representing the tip of the sector (center of the disc to which this sector belongs)
* \param sectorForwardDegrees The orientation of the (center of the) sector from the X-axis
* \param sectorApertureDegrees The "aperture" of the sector, the angle described by the start and end radii of the sector
* \param color The Rgba8 color that the vertexes for the sector should be drawn in
* \sa <a href="_vertex_utils_8hpp.html#ad62de8c113b46420750951484c2666f8">AddVertsForDirectedSector2D</a>
* 
*/
void AddVertsForOrientedSector2D(std::vector<Vertex_PCU>& verts, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius, Rgba8 const& color)
{
	constexpr int NUM_TRIANGLES = 20;
	constexpr int NUM_VERTEXES = NUM_TRIANGLES * 3;
	float degreesIncrementPerVertex = sectorApertureDegrees / static_cast<float>(NUM_VERTEXES);

	Vec2 previousVertexPosition = sectorTip + Vec2::MakeFromPolarDegrees(sectorForwardDegrees - sectorApertureDegrees * 0.5f, sectorRadius);

	for (int vertexIndex = 0; vertexIndex < NUM_VERTEXES; vertexIndex += 3)
	{
		Vec2 sectorTipVertexPosition = sectorTip;
		Vec2 newVertexPosition = sectorTip + Vec2::MakeFromPolarDegrees(sectorForwardDegrees - sectorApertureDegrees * 0.5f + static_cast<float>(vertexIndex + 3) * degreesIncrementPerVertex, sectorRadius);

		Vertex_PCU sectorTipVertex = Vertex_PCU(sectorTip.ToVec3(), color, Vec2::ZERO);
		Vertex_PCU previousVertex = Vertex_PCU(previousVertexPosition.ToVec3(), color, Vec2::ZERO);
		Vertex_PCU newVertex = Vertex_PCU(newVertexPosition.ToVec3(), color, Vec2::ZERO);

		verts.push_back(sectorTipVertex);
		verts.push_back(previousVertex);
		verts.push_back(newVertex);

		previousVertexPosition = newVertexPosition;
	}
}

/*! \brief Adds vertexes for rendering a sector to a list of vertexes
* 
* Pushes back the vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the sector. This function draws a "directed" sector, where the forward vector of the sector is given. This function internally computes the sector orientation from the X-axis and uses the AddVertsForOrientedSector function.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for a sector should be appended
* \param sectorTip A Vec2 representing the tip of the sector (center of the disc to which this sector belongs)
* \param sectorForwardNormal A Vec2 representing the direction of the (center of the) sector
* \param sectorApertureDegrees The "aperture" of the sector, the angle described by the start and end radii of the sector
* \param color The Rgba8 color that the vertexes for the sector should be drawn in
* \sa <a href="_vertex_utils_8hpp.html#ad62de8c113b46420750951484c2666f8">AddVertsForOrientedSector2D</a>
* 
*/
void AddVertsForDirectedSector2D(std::vector<Vertex_PCU>& verts, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius, Rgba8 const& color)
{
	float sectorForwardDegrees = sectorForwardNormal.GetOrientationDegrees();

	AddVertsForOrientedSector2D(verts, sectorTip, sectorForwardDegrees, sectorApertureDegrees, sectorRadius, color);
}

/*! \brief Adds vertexes for rendering an Axis-Aligned Bounding Box 2D
* 
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the AABB2.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the AABB2 should be appended
* \param bounds The AABB2 for which vertexes should be added
* \param color The Rgba8 color that the AABB2 should be drawn in
* \param uvAtMins A Vec2 representing the texture UV coordinates to apply at the bottom left corner of the AABB2
* \param uvAtMaxs A Vec2 representing the texture UV coordinates to apply at the top right corder of the AABB2
* 
*/
void AddVertsForAABB2(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs)
{
	Vec2 vertexBLPosition = bounds.m_mins;
	Vec2 vertexBRPosition = Vec2(bounds.m_maxs.x, bounds.m_mins.y);
	Vec2 vertexTRPosition = bounds.m_maxs;
	Vec2 vertexTLPosition = Vec2(bounds.m_mins.x, bounds.m_maxs.y);

	Vertex_PCU vertexBL = Vertex_PCU(vertexBLPosition.ToVec3(), color, uvAtMins);
	Vertex_PCU vertexBR = Vertex_PCU(vertexBRPosition.ToVec3(), color, Vec2(uvAtMaxs.x, uvAtMins.y));
	Vertex_PCU vertexTR = Vertex_PCU(vertexTRPosition.ToVec3(), color, uvAtMaxs);
	Vertex_PCU vertexTL = Vertex_PCU(vertexTLPosition.ToVec3(), color, Vec2(uvAtMins.x, uvAtMaxs.y));

	verts.push_back(vertexBL);
	verts.push_back(vertexBR);
	verts.push_back(vertexTR);

	verts.push_back(vertexBL);
	verts.push_back(vertexTR);
	verts.push_back(vertexTL);
}

/*! \brief Adds vertexes for rendering an Oriented Bounding Box 2D
* 
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the OBB2.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the OBB2 should be appended
* \param box The OBB2 for which vertexes should be added
* \param color The Rgba8 color that the OBB2 should be drawn in
* 
*/
void AddVertsForOBB2(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color)
{
	Vec2 vertexPositions[4];
	box.GetCornerPoints(vertexPositions);

	Vertex_PCU vertex1 = Vertex_PCU(vertexPositions[0].ToVec3(), color, Vec2::ZERO);
	Vertex_PCU vertex2 = Vertex_PCU(vertexPositions[1].ToVec3(), color, Vec2::ZERO);
	Vertex_PCU vertex3 = Vertex_PCU(vertexPositions[2].ToVec3(), color, Vec2::ZERO);
	Vertex_PCU vertex4 = Vertex_PCU(vertexPositions[3].ToVec3(), color, Vec2::ZERO);

	verts.push_back(vertex1);
	verts.push_back(vertex2);
	verts.push_back(vertex3);

	verts.push_back(vertex1);
	verts.push_back(vertex3);
	verts.push_back(vertex4);
}

/*! \brief Adds vertexes for rendering a line segment 2D
* 
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the line segment.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the line segment should be appended
* \param start A Vec2 representing the start of the line segment
* \param end A Vec2 representing the end of the line segment
* \param thickness The thickness of the line segment
* \param color The Rgba8 color that the line segment should be drawn in
* 
*/
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color)
{
	Vec2 forwardNormal = (end - start).GetNormalized();
	Vec2 leftNormal = forwardNormal.GetRotated90Degrees();

	Vec2 vertex1Position = start - (forwardNormal + leftNormal) * thickness;
	Vec2 vertex2Position = start - (forwardNormal - leftNormal) * thickness;
	Vec2 vertex3Position = end + (forwardNormal - leftNormal) * thickness;
	Vec2 vertex4Position = end + (forwardNormal + leftNormal) * thickness;

	Vertex_PCU vertex1 = Vertex_PCU(vertex1Position.ToVec3(), color, Vec2::ZERO);
	Vertex_PCU vertex2 = Vertex_PCU(vertex2Position.ToVec3(), color, Vec2::ZERO);
	Vertex_PCU vertex3 = Vertex_PCU(vertex3Position.ToVec3(), color, Vec2::ZERO);
	Vertex_PCU vertex4 = Vertex_PCU(vertex4Position.ToVec3(), color, Vec2::ZERO);

	verts.push_back(vertex1);
	verts.push_back(vertex3);
	verts.push_back(vertex4);

	verts.push_back(vertex1);
	verts.push_back(vertex4);
	verts.push_back(vertex2);
}

/*! \brief Adds vertexes for rendering a line segment 2D with a linear gradient from start to end
* 
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the line segment.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the line segment should be appended
* \param start A Vec2 representing the start of the line segment
* \param end A Vec2 representing the end of the line segment
* \param thickness The thickness of the line segment
* \param startColor The Rgba8 color to be applied to the start point
* \param endColor The Rgba8 color to be applied to the end point
* 
*/
void AddVertsForGradientLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& startColor, Rgba8 const& endColor)
{
	Vec2 forwardNormal = (end - start).GetNormalized();
	Vec2 leftNormal = forwardNormal.GetRotated90Degrees();

	Vec2 vertex1Position = start - (forwardNormal + leftNormal) * thickness;
	Vec2 vertex2Position = start - (forwardNormal - leftNormal) * thickness;
	Vec2 vertex3Position = end + (forwardNormal - leftNormal) * thickness;
	Vec2 vertex4Position = end + (forwardNormal + leftNormal) * thickness;

	Vertex_PCU vertex1 = Vertex_PCU(vertex1Position.ToVec3(), startColor, Vec2::ZERO);
	Vertex_PCU vertex2 = Vertex_PCU(vertex2Position.ToVec3(), startColor, Vec2::ZERO);
	Vertex_PCU vertex3 = Vertex_PCU(vertex3Position.ToVec3(), endColor, Vec2::ZERO);
	Vertex_PCU vertex4 = Vertex_PCU(vertex4Position.ToVec3(), endColor, Vec2::ZERO);

	verts.push_back(vertex1);
	verts.push_back(vertex2);
	verts.push_back(vertex3);

	verts.push_back(vertex2);
	verts.push_back(vertex3);
	verts.push_back(vertex4);
}

/*! \brief Adds vertexes for rendering a 2D arrow
* 
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the arrow.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the arrow should be appended
* \param tailPos A Vec2 representing the start of the arrow
* \param tipPos A Vec2 representing the end of the arrow
* \param arrowSize The length of the line segments forming the arrow shape
* \param lineThickness The thickness of the line segments (for both the line segment from start to end and those forming the arrow shape)
* \param color The Rgba8 color that the arrow should be drawn in
* 
*/
void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 const& tailPos, Vec2 const& tipPos, float arrowSize, float lineThickness, Rgba8 const& color)
{
	AddVertsForLineSegment2D(verts, tailPos, tipPos, lineThickness, color);

	Vec2 arrowDirection = (tipPos - tailPos).GetNormalized();
	AddVertsForLineSegment2D(verts, tipPos, tipPos + arrowDirection.GetRotatedDegrees(135.f) * arrowSize, lineThickness, color);
	AddVertsForLineSegment2D(verts, tipPos, tipPos + arrowDirection.GetRotatedDegrees(-135.f) * arrowSize, lineThickness, color);
}

void AddVertsForConvexPoly2(std::vector<Vertex_PCU>& verts, ConvexPoly2 const& convexPoly, Rgba8 const& color)
{
	std::vector<Vec2> vertexes = convexPoly.GetVertexes();
	
	if (vertexes.size() < 3)
	{
		ERROR_AND_DIE("Attempted to add verts for invalid convex poly");
	}

	Vertex_PCU zerothVertex(vertexes[0].ToVec3(), color, Vec2::ZERO);
	for (int vertexIndex = 1; vertexIndex < vertexes.size() - 1; vertexIndex++)
	{
		verts.push_back(zerothVertex);
		verts.push_back(Vertex_PCU(vertexes[vertexIndex].ToVec3(), color, Vec2::ZERO));
		verts.push_back(Vertex_PCU(vertexes[vertexIndex + 1].ToVec3(), color, Vec2::ZERO));
	}
}

void AddOutlineVertsForConvexPoly2(std::vector<Vertex_PCU>& verts, ConvexPoly2 const& convexPoly, float thickness, Rgba8 const& color)
{
	std::vector<Vec2> vertexes = convexPoly.GetVertexes();

	if (vertexes.size() < 3)
	{
		ERROR_AND_DIE("Attempted to add outline verts for invalid convex poly");
	}

	for (int vertexIndex = 0; vertexIndex < vertexes.size() - 1; vertexIndex++)
	{
		AddVertsForLineSegment2D(verts, vertexes[vertexIndex], vertexes[vertexIndex + 1], thickness, color);
	}

	// Add line segment from last vertex to first to close the convex poly
	AddVertsForLineSegment2D(verts, vertexes[vertexes.size() - 1], vertexes[0], thickness, color);
}

/*! \brief Adds vertexes for rendering a 3D quad
* 
* Pushes back six vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the quad.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the quad should be appended
* \param bottomLeft A Vec3 representing the bottom left corner of the quad
* \param bottomRight A Vec3 representing the bottom right corner of the quad
* \param topRight A Vec3 representing the top right corner of the quad
* \param topLeft A Vec3 representing the top left corner of the quad
* \param color (Optional) The Rgba8 color that the quad should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates for the quad. Defaults to AABB2::ZERO_TO_ONE
* 
*/
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vertex_PCU bottomLeftVertex = Vertex_PCU(bottomLeft, color, uvCoords.m_mins);
	Vertex_PCU bottomRightVertex = Vertex_PCU(bottomRight, color, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y));
	Vertex_PCU topRightVertex = Vertex_PCU(topRight, color, uvCoords.m_maxs);
	Vertex_PCU topLeftVertex = Vertex_PCU(topLeft, color, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y));

	verts.push_back(bottomLeftVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);

	verts.push_back(bottomLeftVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topLeftVertex);
}

/*! \brief Adds vertexes for rendering a 3D quad
*
* Pushes back six vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the quad.
* \param verts The list of Vertex_PCUTBN objects, passed by reference, to which the vertexes for the quad should be appended
* \param bottomLeft A Vec3 representing the bottom left corner of the quad
* \param bottomRight A Vec3 representing the bottom right corner of the quad
* \param topRight A Vec3 representing the top right corner of the quad
* \param topLeft A Vec3 representing the top left corner of the quad
* \param color (Optional) The Rgba8 color that the quad should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates for the quad. Defaults to AABB2::ZERO_TO_ONE
*
*/
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 normalBL = CrossProduct3D(bottomRight - bottomLeft, topLeft - bottomLeft).GetNormalized();
	Vec3 normalBR = CrossProduct3D(topRight - bottomRight, bottomLeft - bottomRight).GetNormalized();
	Vec3 normalTR = CrossProduct3D(topLeft - topRight, bottomRight - topRight).GetNormalized();
	Vec3 normalTL = CrossProduct3D(bottomLeft - topLeft, topRight - topLeft).GetNormalized();

	Vertex_PCUTBN bottomLeftVertex = Vertex_PCUTBN(bottomLeft, color, uvCoords.m_mins, Vec3::ZERO, Vec3::ZERO, normalBL);
	Vertex_PCUTBN bottomRightVertex = Vertex_PCUTBN(bottomRight, color, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y), Vec3::ZERO, Vec3::ZERO, normalBR);
	Vertex_PCUTBN topRightVertex = Vertex_PCUTBN(topRight, color, uvCoords.m_maxs, Vec3::ZERO, Vec3::ZERO, normalTR);
	Vertex_PCUTBN topLeftVertex = Vertex_PCUTBN(topLeft, color, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y), Vec3::ZERO, Vec3::ZERO, normalTL);

	verts.push_back(bottomLeftVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);

	verts.push_back(bottomLeftVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topLeftVertex);
}

/*! \brief Adds vertexes for rendering a 3D quad
*
* Pushes back six vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the quad.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the quad should be appended
* \param bottomLeft A Vec3 representing the bottom left corner of the quad
* \param bottomRight A Vec3 representing the bottom right corner of the quad
* \param topRight A Vec3 representing the top right corner of the quad
* \param topLeft A Vec3 representing the top left corner of the quad
* \param color (Optional) The Rgba8 color that the quad should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates for the quad. Defaults to AABB2::ZERO_TO_ONE
*
*/
void AddVertsForGradientQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& BLColor, Rgba8 const& BRColor, Rgba8 const& TRColor, Rgba8 const& TLColor, AABB2 const& uvCoords)
{
	Vertex_PCU bottomLeftVertex = Vertex_PCU(bottomLeft, BLColor, uvCoords.m_mins);
	Vertex_PCU bottomRightVertex = Vertex_PCU(bottomRight, BRColor, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y));
	Vertex_PCU topRightVertex = Vertex_PCU(topRight, TRColor, uvCoords.m_maxs);
	Vertex_PCU topLeftVertex = Vertex_PCU(topLeft, TLColor, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y));

	verts.push_back(bottomLeftVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);

	verts.push_back(bottomLeftVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topLeftVertex);
}

/*! \brief Adds vertexes and indexes for rendering an indexed 3D quad
* 
* Pushes back four vertexes and six indexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the quad.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the quad should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes for the quad should be appended
* \param bottomLeft A Vec3 representing the bottom left corner of the quad
* \param bottomRight A Vec3 representing the bottom right corner of the quad
* \param topRight A Vec3 representing the top right corner of the quad
* \param topLeft A Vec3 representing the top left corner of the quad
* \param color (Optional) The Rgba8 color that the quad should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates for the quad. Defaults to AABB2::ZERO_TO_ONE
* \sa IndexBuffer
* 
*/
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vertex_PCU bottomLeftVertex = Vertex_PCU(bottomLeft, color, uvCoords.m_mins);
	Vertex_PCU bottomRightVertex = Vertex_PCU(bottomRight, color, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y));
	Vertex_PCU topRightVertex = Vertex_PCU(topRight, color, uvCoords.m_maxs);
	Vertex_PCU topLeftVertex = Vertex_PCU(topLeft, color, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y));

	unsigned int indexOffset = (unsigned int)verts.size();

	verts.push_back(bottomLeftVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topLeftVertex);


	indexes.push_back(indexOffset + 0);
	indexes.push_back(indexOffset + 1);
	indexes.push_back(indexOffset + 2);

	indexes.push_back(indexOffset + 0);
	indexes.push_back(indexOffset + 2);
	indexes.push_back(indexOffset + 3);
}

/*! \brief Adds PCUTBN vertexes and indexes for rendering an indexed 3D quad
*
* \warning This function is incomplete in that it does not calculate tangents and bitangents for the vertexes. It only calcualtes the normals and uses zero vectors for the missing components.
* Pushes back four PCUTBN vertexes and six indexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the quad.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the quad should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes for the quad should be appended
* \param bottomLeft A Vec3 representing the bottom left corner of the quad
* \param bottomRight A Vec3 representing the bottom right corner of the quad
* \param topRight A Vec3 representing the top right corner of the quad
* \param topLeft A Vec3 representing the top left corner of the quad
* \param color (Optional) The Rgba8 color that the quad should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates for the quad. Defaults to AABB2::ZERO_TO_ONE
* \sa IndexBuffer
* \sa Vertex_PCUTBN
*
*/
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 normalBL = CrossProduct3D(bottomRight - bottomLeft, topLeft - bottomLeft).GetNormalized();
	Vec3 normalBR = CrossProduct3D(topRight - bottomRight, bottomLeft - bottomRight).GetNormalized();
	Vec3 normalTR = CrossProduct3D(topLeft - topRight, bottomRight - topRight).GetNormalized();
	Vec3 normalTL = CrossProduct3D(bottomLeft - topLeft, topRight - topLeft).GetNormalized();

	Vertex_PCUTBN bottomLeftVertex = Vertex_PCUTBN(bottomLeft, color, uvCoords.m_mins, Vec3::ZERO, Vec3::ZERO, normalBL);
	Vertex_PCUTBN bottomRightVertex = Vertex_PCUTBN(bottomRight, color, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y), Vec3::ZERO, Vec3::ZERO, normalBR);
	Vertex_PCUTBN topRightVertex = Vertex_PCUTBN(topRight, color, uvCoords.m_maxs, Vec3::ZERO, Vec3::ZERO, normalTR);
	Vertex_PCUTBN topLeftVertex = Vertex_PCUTBN(topLeft, color, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y), Vec3::ZERO, Vec3::ZERO, normalTL);

	unsigned int indexOffset = (unsigned int)verts.size();

	verts.push_back(bottomLeftVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topLeftVertex);


	indexes.push_back(indexOffset + 0);
	indexes.push_back(indexOffset + 1);
	indexes.push_back(indexOffset + 2);

	indexes.push_back(indexOffset + 0);
	indexes.push_back(indexOffset + 2);
	indexes.push_back(indexOffset + 3);
}

/*! \brief Adds vertexes for a "rounded quad"
*
* A rounded quad is drawn as two quads sharing a common axis along the up direction of formed by the quad. Not recommended for degenerate quads
* \param verts The list of Vertex_PCUTBN objects, passed by reference, to which the vertexes for the quad(s) should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes should be appended
* \param bottomLeft A Vec3 representing the bottom left corner of the quad
* \param bottomRight A Vec3 representing the bottom right corner of the quad
* \param topRight A Vec3 representing the top right corner of the quad
* \param topLeft A Vec3 representing the top left corner of the quad
* \param color (Optional) The Rgba8 color that the quad should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates for the quad. Defaults to AABB2::ZERO_TO_ONE
*
*/
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 bottomMid = bottomLeft + (bottomRight - bottomLeft).GetNormalized() * (bottomRight - bottomLeft).GetLength() * 0.5f;
	Vec3 topMid = topLeft + (topRight - topLeft).GetNormalized() * (topRight - topLeft).GetLength() * 0.5f;

	Vec3 bottomMidNormalLeftQuad = CrossProduct3D(topMid - bottomMid, bottomLeft - bottomMid).GetNormalized();
	Vec3 bottomMidNormalRightQuad = CrossProduct3D(bottomRight - bottomMid, topMid - bottomMid).GetNormalized();
	Vec3 bottomMidNormal = (bottomMidNormalLeftQuad + bottomMidNormalRightQuad) * 0.5f;
	Vec3 topMidNormalLeftQuad = CrossProduct3D(topLeft - topMid, bottomMid - topMid).GetNormalized();
	Vec3 topMidNormalRightQuad = CrossProduct3D(bottomMid - topMid, topRight - topMid).GetNormalized();
	Vec3 topMidNormal = (topMidNormalLeftQuad + topMidNormalRightQuad) * 0.5f;

	Vec3 topLeftNormal = (topLeft - topMid).GetNormalized();
	Vec3 topRightNormal = (topRight - topMid).GetNormalized();
	Vec3 bottomLeftNormal = (bottomLeft - bottomMid).GetNormalized();
	Vec3 bottomRightNormal = (bottomRight - bottomMid).GetNormalized();

	Vertex_PCUTBN bottomLeftVertex = Vertex_PCUTBN(bottomLeft, color, uvCoords.m_mins, Vec3::ZERO, Vec3::ZERO, bottomLeftNormal);
	Vertex_PCUTBN bottomRightVertex = Vertex_PCUTBN(bottomRight, color, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y), Vec3::ZERO, Vec3::ZERO, bottomRightNormal);
	Vertex_PCUTBN bottomMidVertex = Vertex_PCUTBN(bottomMid, color, Vec2(uvCoords.GetCenter().x, uvCoords.m_mins.y), Vec3::ZERO, Vec3::ZERO, bottomMidNormal);
	Vertex_PCUTBN topRightVertex = Vertex_PCUTBN(topRight, color, uvCoords.m_maxs, Vec3::ZERO, Vec3::ZERO, topRightNormal);
	Vertex_PCUTBN topLeftVertex = Vertex_PCUTBN(topLeft, color, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y), Vec3::ZERO, Vec3::ZERO, topLeftNormal);
	Vertex_PCUTBN topMidVertex = Vertex_PCUTBN(topMid, color, Vec2(uvCoords.GetCenter().x, uvCoords.m_maxs.y), Vec3::ZERO, Vec3::ZERO, topMidNormal);

	verts.push_back(bottomLeftVertex); // index = 0
	verts.push_back(bottomMidVertex);
	verts.push_back(topMidVertex);
	verts.push_back(bottomLeftVertex);
	verts.push_back(topMidVertex);
	verts.push_back(topLeftVertex);

	verts.push_back(bottomMidVertex);
	verts.push_back(bottomRightVertex);
	verts.push_back(topRightVertex);
	verts.push_back(bottomMidVertex);
	verts.push_back(topRightVertex);
	verts.push_back(topMidVertex);
}

/*! \brief Adds vertexes and indexes for a "rounded quad"
* 
* A rounded quad is drawn as two quads sharing a common axis along the up direction of formed by the quad. Not recommended for degenerate quads
* \param verts The list of Vertex_PCUTBN objects, passed by reference, to which the vertexes for the quad(s) should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes should be appended
* \param bottomLeft A Vec3 representing the bottom left corner of the quad
* \param bottomRight A Vec3 representing the bottom right corner of the quad
* \param topRight A Vec3 representing the top right corner of the quad
* \param topLeft A Vec3 representing the top left corner of the quad
* \param color (Optional) The Rgba8 color that the quad should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates for the quad. Defaults to AABB2::ZERO_TO_ONE
* 
*/
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 bottomMid = bottomLeft + (bottomRight - bottomLeft).GetNormalized() * (bottomRight - bottomLeft).GetLength();
	Vec3 topMid = topLeft + (topRight - topLeft).GetNormalized() * (topRight - topLeft).GetLength();

	Vec3 bottomMidNormalLeftQuad = CrossProduct3D(topMid - bottomMid, bottomLeft - bottomMid).GetNormalized();
	Vec3 bottomMidNormalRightQuad = CrossProduct3D(bottomRight - bottomMid, topMid - bottomMid).GetNormalized();
	Vec3 bottomMidNormal = (bottomMidNormalLeftQuad + bottomMidNormalRightQuad) * 0.5f;
	Vec3 topMidNormalLeftQuad = CrossProduct3D(topLeft - topMid, bottomMid - topMid).GetNormalized();
	Vec3 topMidNormalRightQuad = CrossProduct3D(bottomMid - topMid, topRight - topMid).GetNormalized();
	Vec3 topMidNormal = (topMidNormalLeftQuad + topMidNormalRightQuad) * 0.5f;

	Vec3 topLeftNormal = (topLeft - topMid).GetNormalized();
	Vec3 topRightNormal = (topRight - topMid).GetNormalized();
	Vec3 bottomLeftNormal = (bottomLeft - bottomMid).GetNormalized();
	Vec3 bottomRightNormal = (bottomRight - bottomMid).GetNormalized();

	Vertex_PCUTBN bottomLeftVertex = Vertex_PCUTBN(bottomLeft, color, uvCoords.m_mins, Vec3::ZERO, Vec3::ZERO, bottomLeftNormal);
	Vertex_PCUTBN bottomRightVertex = Vertex_PCUTBN(bottomRight, color, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y), Vec3::ZERO, Vec3::ZERO, bottomRightNormal);
	Vertex_PCUTBN bottomMidVertex = Vertex_PCUTBN(bottomMid, color, Vec2(uvCoords.GetCenter().x, uvCoords.m_mins.y), Vec3::ZERO, Vec3::ZERO, bottomMidNormal);
	Vertex_PCUTBN topRightVertex = Vertex_PCUTBN(topRight, color, uvCoords.m_maxs, Vec3::ZERO, Vec3::ZERO, topRightNormal);
	Vertex_PCUTBN topLeftVertex = Vertex_PCUTBN(topLeft, color, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y), Vec3::ZERO, Vec3::ZERO, topLeftNormal);
	Vertex_PCUTBN topMidVertex = Vertex_PCUTBN(topMid, color, Vec2(uvCoords.GetCenter().x, uvCoords.m_maxs.y), Vec3::ZERO, Vec3::ZERO, topMidNormal);

	unsigned int indexOffset = (unsigned int)verts.size();

	verts.push_back(bottomLeftVertex); // index = 0
	verts.push_back(topLeftVertex); // index = 1
	verts.push_back(bottomRightVertex); // index = 2
	verts.push_back(topRightVertex); // index = 3
	verts.push_back(bottomMidVertex); // index = 4
	verts.push_back(topMidVertex); // index = 5

	indexes.push_back(indexOffset + 0);
	indexes.push_back(indexOffset + 4);
	indexes.push_back(indexOffset + 5);
	indexes.push_back(indexOffset + 0);
	indexes.push_back(indexOffset + 5);
	indexes.push_back(indexOffset + 1);

	indexes.push_back(indexOffset + 4);
	indexes.push_back(indexOffset + 2);
	indexes.push_back(indexOffset + 3);
	indexes.push_back(indexOffset + 4);
	indexes.push_back(indexOffset + 3);
	indexes.push_back(indexOffset + 5);
}

/* ! \brief Adds vertexes for rendering a ring in 3D
*/
void AddVertsForRing3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float thickness, EulerAngles const& orientation, Rgba8 const& color, int numTrapezoids)
{
	UNUSED(orientation);

	const float thetaIncrementDegrees = 360.f / (float)(numTrapezoids);
	const float innerRadius = radius - thickness * 0.5f;
	const float outerRadius = radius + thickness * 0.5f;

	for (int trapIndex = 0; trapIndex < numTrapezoids; trapIndex++)
	{
		float startThetaDegrees = (float)(trapIndex)*thetaIncrementDegrees;
		float endThetaDegrees = (float)(trapIndex + 1) * thetaIncrementDegrees;

		Vec3 innerStartVertexPos = center + Vec2::MakeFromPolarDegrees(startThetaDegrees, innerRadius).ToVec3();
		Vec3 outerStartVertexPos = center + Vec2::MakeFromPolarDegrees(startThetaDegrees, outerRadius).ToVec3();
		Vec3 innerEndVertexPos = center + Vec2::MakeFromPolarDegrees(endThetaDegrees, innerRadius).ToVec3();
		Vec3 outerEndVertexPos = center + Vec2::MakeFromPolarDegrees(endThetaDegrees, outerRadius).ToVec3();

		verts.push_back(Vertex_PCU(Vec3(innerStartVertexPos.x, innerStartVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerStartVertexPos.x, outerStartVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(innerEndVertexPos.x, innerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerStartVertexPos.x, outerStartVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(outerEndVertexPos.x, outerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
		verts.push_back(Vertex_PCU(Vec3(innerEndVertexPos.x, innerEndVertexPos.y, 0.f), color, Vec2(0.f, 0.f)));
	}
}

void AddVertsForDisc3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color, int numTris)
{
	const float DEGREES_INCREMENT_PER_VERTEX = 360.f / static_cast<float>(numTris);

	for (int triIndex = 0; triIndex < numTris; triIndex++)
	{
		Vec3 startVertexPosition = center + Vec2::MakeFromPolarDegrees(triIndex * DEGREES_INCREMENT_PER_VERTEX, radius).ToVec3();
		Vec3 endVertexPosition = center + Vec2::MakeFromPolarDegrees(static_cast<float>(triIndex + 1) * DEGREES_INCREMENT_PER_VERTEX, radius).ToVec3();
		Vertex_PCU centerVertex = Vertex_PCU(center, color, Vec2::ZERO);
		Vertex_PCU startVertex = Vertex_PCU(startVertexPosition, color, Vec2::ZERO);
		Vertex_PCU endVertex = Vertex_PCU(endVertexPosition, color, Vec2::ZERO);

		verts.push_back(centerVertex);
		verts.push_back(startVertex);
		verts.push_back(endVertex);
	}
}

/*! \brief Adds vertexes for rendering an Axis-Aligned Bounding Box 3D
* 
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the AABB3 with the texture being applied individually on each face of the AABB3.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the AABB3 should be appended
* \param bounds The AABB3 for which vertexes should be appended
* \param color (Optional) The Rgba8 color that the AABB3 should be drawn in. Defaults to Rgba8::WHTIE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates to be applied on each face of the AABB3. Defaults to AABB2::ZERO_TO_ONE
* 
*/
void AddVertsForAABB3(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 const& mins = bounds.m_mins;
	Vec3 const& maxs = bounds.m_maxs;

	Vec3 BLF = Vec3(mins.x, maxs.y, mins.z);
	Vec3 BRF = Vec3(mins.x, mins.y, mins.z);
	Vec3 TRF = Vec3(mins.x, mins.y, maxs.z);
	Vec3 TLF = Vec3(mins.x, maxs.y, maxs.z);
	Vec3 BLB = Vec3(maxs.x, maxs.y, mins.z);
	Vec3 BRB = Vec3(maxs.x, mins.y, mins.z);
	Vec3 TRB = Vec3(maxs.x, mins.y, maxs.z);
	Vec3 TLB = Vec3(maxs.x, maxs.y, maxs.z);

	AddVertsForQuad3D(verts, BRB, BLB, TLB, TRB, color, uvCoords); // +X
	AddVertsForQuad3D(verts, BLF, BRF, TRF, TLF, color, uvCoords); // -X
	AddVertsForQuad3D(verts, BLB, BLF, TLF, TLB, color, uvCoords); // +Y
	AddVertsForQuad3D(verts, BRF, BRB, TRB, TRF, color, uvCoords); // -Y
	AddVertsForQuad3D(verts, TLF, TRF, TRB, TLB, color, uvCoords); // +Z
	AddVertsForQuad3D(verts, BLB, BRB, BRF, BLF, color, uvCoords); // -Z
}

/*! \brief Adds vertexes for rendering an Axis-Aligned Bounding Box 3D
*
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the AABB3 with the texture being applied individually on each face of the AABB3.
* \param verts The list of Vertex_PCUTBN objects, passed by reference, to which the vertexes for the AABB3 should be appended
* \param bounds The AABB3 for which vertexes should be appended
* \param color (Optional) The Rgba8 color that the AABB3 should be drawn in. Defaults to Rgba8::WHTIE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates to be applied on each face of the AABB3. Defaults to AABB2::ZERO_TO_ONE
*
*/
void AddVertsForAABB3(std::vector<Vertex_PCUTBN>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 const& mins = bounds.m_mins;
	Vec3 const& maxs = bounds.m_maxs;

	Vec3 BLF = Vec3(mins.x, maxs.y, mins.z);
	Vec3 BRF = Vec3(mins.x, mins.y, mins.z);
	Vec3 TRF = Vec3(mins.x, mins.y, maxs.z);
	Vec3 TLF = Vec3(mins.x, maxs.y, maxs.z);
	Vec3 BLB = Vec3(maxs.x, maxs.y, mins.z);
	Vec3 BRB = Vec3(maxs.x, mins.y, mins.z);
	Vec3 TRB = Vec3(maxs.x, mins.y, maxs.z);
	Vec3 TLB = Vec3(maxs.x, maxs.y, maxs.z);

	AddVertsForQuad3D(verts, BRB, BLB, TLB, TRB, color, uvCoords); // +X
	AddVertsForQuad3D(verts, BLF, BRF, TRF, TLF, color, uvCoords); // -X
	AddVertsForQuad3D(verts, BLB, BLF, TLF, TLB, color, uvCoords); // +Y
	AddVertsForQuad3D(verts, BRF, BRB, TRB, TRF, color, uvCoords); // -Y
	AddVertsForQuad3D(verts, TLF, TRF, TRB, TLB, color, uvCoords); // +Z
	AddVertsForQuad3D(verts, BLB, BRB, BRF, BLF, color, uvCoords); // -Z
}

/*! \brief Adds vertexes and indexes for rendering an indexed Axis-Aligned Bounding Box 3D
* 
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the AABB3 with the texture being applied individually on each face of the AABB3.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the AABB3 should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes should be appended
* \param bounds The AABB3 for which vertexes and indexes should be added
* \param color (Optional) The Rgba8 color that the AABB3 should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates to be applied on each face of the AABB3. Defaults to AABB2::ZERO_TO_ONE
* \sa IndexBuffer
* 
*/
void AddVertsForAABB3(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 const& mins = bounds.m_mins;
	Vec3 const& maxs = bounds.m_maxs;

	Vec3 BLF = Vec3(mins.x, maxs.y, mins.z);
	Vec3 BRF = Vec3(mins.x, mins.y, mins.z);
	Vec3 TRF = Vec3(mins.x, mins.y, maxs.z);
	Vec3 TLF = Vec3(mins.x, maxs.y, maxs.z);
	Vec3 BLB = Vec3(maxs.x, maxs.y, mins.z);
	Vec3 BRB = Vec3(maxs.x, mins.y, mins.z);
	Vec3 TRB = Vec3(maxs.x, mins.y, maxs.z);
	Vec3 TLB = Vec3(maxs.x, maxs.y, maxs.z);

	AddVertsForQuad3D(verts, indexes, BRB, BLB, TLB, TRB, color, uvCoords); // +X
	AddVertsForQuad3D(verts, indexes, BLF, BRF, TRF, TLF, color, uvCoords); // -X
	AddVertsForQuad3D(verts, indexes, BLB, BLF, TLF, TLB, color, uvCoords); // +Y
	AddVertsForQuad3D(verts, indexes, BRF, BRB, TRB, TRF, color, uvCoords); // -Y
	AddVertsForQuad3D(verts, indexes, TLF, TRF, TRB, TLB, color, uvCoords); // +Z
	AddVertsForQuad3D(verts, indexes, BLB, BRB, BRF, BLF, color, uvCoords); // -Z
}

/*! \brief Adds vertexes and indexes for rendering an indexed Axis-Aligned Bounding Box 3D
*
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the AABB3 with the texture being applied individually on each face of the AABB3.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the AABB3 should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes should be appended
* \param bounds The AABB3 for which vertexes and indexes should be added
* \param color (Optional) The Rgba8 color that the AABB3 should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates to be applied on each face of the AABB3. Defaults to AABB2::ZERO_TO_ONE
* \sa IndexBuffer
*
*/
void AddVertsForAABB3(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords)
{
	Vec3 const& mins = bounds.m_mins;
	Vec3 const& maxs = bounds.m_maxs;

	Vec3 BLF = Vec3(mins.x, maxs.y, mins.z);
	Vec3 BRF = Vec3(mins.x, mins.y, mins.z);
	Vec3 TRF = Vec3(mins.x, mins.y, maxs.z);
	Vec3 TLF = Vec3(mins.x, maxs.y, maxs.z);
	Vec3 BLB = Vec3(maxs.x, maxs.y, mins.z);
	Vec3 BRB = Vec3(maxs.x, mins.y, mins.z);
	Vec3 TRB = Vec3(maxs.x, mins.y, maxs.z);
	Vec3 TLB = Vec3(maxs.x, maxs.y, maxs.z);

	AddVertsForQuad3D(verts, indexes, BRB, BLB, TLB, TRB, color, uvCoords); // +X
	AddVertsForQuad3D(verts, indexes, BLF, BRF, TRF, TLF, color, uvCoords); // -X
	AddVertsForQuad3D(verts, indexes, BLB, BLF, TLF, TLB, color, uvCoords); // +Y
	AddVertsForQuad3D(verts, indexes, BRF, BRB, TRB, TRF, color, uvCoords); // -Y
	AddVertsForQuad3D(verts, indexes, TLF, TRF, TRB, TLB, color, uvCoords); // +Z
	AddVertsForQuad3D(verts, indexes, BLB, BRB, BRF, BLF, color, uvCoords); // -Z
}

/*! \brief Adds vertexes for rendering an Oriented Bounding Box 3D
*
* Pushes back the vertexes to the end of the provided list. Game code may reserve vertexes but not construct placeholder instances in the list. This function supports textures on the AABB3 with the texture being applied individually on each face of the AABB3.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the AABB3 should be appended
* \param bounds The AABB3 for which vertexes should be appended
* \param color (Optional) The Rgba8 color that the AABB3 should be drawn in. Defaults to Rgba8::WHTIE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates to be applied on each face of the AABB3. Defaults to AABB2::ZERO_TO_ONE
*
*/
void AddVertsForOBB3(std::vector<Vertex_PCU>& verts, OBB3 const& orientedBox, Rgba8 const& color, AABB2 const& uvCoords)
{
	std::vector<Vertex_PCU> tempVerts;
	AddVertsForAABB3(tempVerts, AABB3(-orientedBox.m_halfDimensions, orientedBox.m_halfDimensions), color, uvCoords);
	Mat44 obbTransformMatrix = Mat44(orientedBox.m_iBasis, orientedBox.m_jBasis, orientedBox.m_kBasis, orientedBox.m_center);
	TransformVertexArray3D(tempVerts, obbTransformMatrix);
	for (int vertexIndex = 0; vertexIndex < (int)tempVerts.size(); vertexIndex++)
	{
		verts.push_back(tempVerts[vertexIndex]);
	}
}

/*! \brief Adds vertexes for rendering a sphere
* 
* Pushes back vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function supports textures on the sphere.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the sphere should be appended
* \param center A Vec3 representing the center of the sphere
* \param radius The radius of the sphere
* \param color (Optional) The Rgba8 color that the sphere should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates to be applied to the sphere. Defaults to AABB2::ZERO_TO_ONE
* \param numStacks (Optional) The number of stacks to be used when drawing the sphere. Defaults to 8. The number of slices is calculated by multiplying the number of stacks by 2.
* 
*/
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& uvCoords, int numStacks, int numSlices)
{
	Vec3 topVertexPos = center + radius * Vec3::SKYWARD;
	Vec3 bottomVertexPos = center + radius * Vec3::GROUNDWARD;

	float degreeIncrementPerSlice = 360.f / (float)numSlices;

	for (int stack = 0; stack < numStacks; stack++)
	{
		for (int slice = 0; slice < numSlices; slice++)
		{
			Vec3 BL = center + Vec3::MakeFromPolarDegrees((float)(slice) * degreeIncrementPerSlice, RangeMap((float)stack, 0.f, (float)numStacks, 90.f, -90.f), radius);
			Vec3 BR = center + Vec3::MakeFromPolarDegrees((float)(slice + 1) * degreeIncrementPerSlice, RangeMap((float)stack, 0.f, (float)numStacks, 90.f, -90.f), radius);
			Vec3 TR = center + Vec3::MakeFromPolarDegrees((float)(slice + 1) * degreeIncrementPerSlice, RangeMap((float)stack + 1.f, 0.f, (float)numStacks, 90.f, -90.f), radius);
			Vec3 TL = center + Vec3::MakeFromPolarDegrees((float)(slice) * degreeIncrementPerSlice, RangeMap((float)stack + 1.f, 0.f, (float)numStacks, 90.f, -90.f), radius);
		
			float uMin = RangeMap((float)slice, 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
			float vMin = RangeMap((float)stack, 0.f, (float)numStacks, uvCoords.m_mins.y, uvCoords.m_maxs.y);
			float uMax = RangeMap((float)(slice + 1), 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
			float vMax = RangeMap((float)(stack + 1), 0.f, (float)numStacks, uvCoords.m_mins.y, uvCoords.m_maxs.y);

			AddVertsForQuad3D(verts, BL, BR, TR, TL, color, AABB2(Vec2(uMin, vMin), Vec2(uMax, vMax)));
		}
	}
}

/*! \brief Adds vertexes for rendering a sphere
*
* Pushes back vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function supports textures on the sphere.
* \param verts The list of Vertex_PCUTBN objects, passed by reference, to which the vertexes for the sphere should be appended
* \param center A Vec3 representing the center of the sphere
* \param radius The radius of the sphere
* \param color (Optional) The Rgba8 color that the sphere should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) An AABB2 representing the texture UV coordinates to be applied to the sphere. Defaults to AABB2::ZERO_TO_ONE
* \param numStacks (Optional) The number of stacks to be used when drawing the sphere. Defaults to 8. The number of slices is calculated by multiplying the number of stacks by 2.
*
*/
void AddVertsForSphere3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& uvCoords, int numStacks, int numSlices)
{
	Vec3 topVertexPos = center + radius * Vec3::SKYWARD;
	Vec3 bottomVertexPos = center + radius * Vec3::GROUNDWARD;

	float degreeIncrementPerSlice = 360.f / (float)numSlices;

	for (int stack = 0; stack < numStacks; stack++)
	{
		for (int slice = 0; slice < numSlices; slice++)
		{
			Vec3 BL = center + Vec3::MakeFromPolarDegrees((float)(slice)*degreeIncrementPerSlice, RangeMap((float)stack, 0.f, (float)numStacks, 90.f, -90.f), radius);
			Vec3 BR = center + Vec3::MakeFromPolarDegrees((float)(slice + 1) * degreeIncrementPerSlice, RangeMap((float)stack, 0.f, (float)numStacks, 90.f, -90.f), radius);
			Vec3 TR = center + Vec3::MakeFromPolarDegrees((float)(slice + 1) * degreeIncrementPerSlice, RangeMap((float)stack + 1.f, 0.f, (float)numStacks, 90.f, -90.f), radius);
			Vec3 TL = center + Vec3::MakeFromPolarDegrees((float)(slice)*degreeIncrementPerSlice, RangeMap((float)stack + 1.f, 0.f, (float)numStacks, 90.f, -90.f), radius);

			Vec3 normalBL = (BL - center).GetNormalized();
			Vec3 normalBR = (BR - center).GetNormalized();
			Vec3 normalTL = (TL - center).GetNormalized();
			Vec3 normalTR = (TR - center).GetNormalized();

			float uMin = RangeMap((float)slice, 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
			float vMin = RangeMap((float)stack, 0.f, (float)numStacks, uvCoords.m_mins.y, uvCoords.m_maxs.y);
			float uMax = RangeMap((float)(slice + 1), 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
			float vMax = RangeMap((float)(stack + 1), 0.f, (float)numStacks, uvCoords.m_mins.y, uvCoords.m_maxs.y);

			Vertex_PCUTBN bottomLeftVertex = Vertex_PCUTBN(BL, color, Vec2(uMin, vMin), Vec3::ZERO, Vec3::ZERO, normalBL);
			Vertex_PCUTBN bottomRightVertex = Vertex_PCUTBN(BR, color, Vec2(uMax, vMin), Vec3::ZERO, Vec3::ZERO, normalBR);
			Vertex_PCUTBN topRightVertex = Vertex_PCUTBN(TR, color, Vec2(uMax, vMax), Vec3::ZERO, Vec3::ZERO, normalTR);
			Vertex_PCUTBN topLeftVertex = Vertex_PCUTBN(TL, color, Vec2(uMin, vMax), Vec3::ZERO, Vec3::ZERO, normalTL);

			verts.push_back(bottomLeftVertex);
			verts.push_back(bottomRightVertex);
			verts.push_back(topRightVertex);

			verts.push_back(bottomLeftVertex);
			verts.push_back(topRightVertex);
			verts.push_back(topLeftVertex);
		}
	}
}

/*! \brief Adds vertexes and indexes for rendering an indexed sphere
*
* Pushes back vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the sphere.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the sphere should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes for the sphere should be appended
* \param center A Vec3 representing the center of the sphere
* \param radius The radius of the sphere
* \param color (Optional) The Rgba8 color that the sphere should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) (UNUSED) An AABB2 representing the texture UV coordinates to be applied to the sphere. Defaults to AABB2::ZERO_TO_ONE
* \param numSlices (Optional) The number of slices to be used when drawing the sphere. Defaults to 16
* \param numStacks (Optional) The number of stacks to be used when drawing the sphere. Defaults to 8
* \sa IndexBuffer
*
*/
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& uvCoords, int numSlices, int numStacks)
{
	UNUSED(uvCoords);

	float degreeIncrementPerSlice = 360.f / (float)numSlices;

	for (int stack = 0; stack < numStacks + 1; stack++)
	{
		for (int slice = 0; slice < numSlices; slice++)
		{
			Vec3 vertexPosition = center + Vec3::MakeFromPolarDegrees((float)slice * degreeIncrementPerSlice, RangeMap((float)stack, 0.f, (float)numStacks, 90.f, -90.f), radius);
			verts.push_back(Vertex_PCU(vertexPosition, color, Vec2::ZERO));

			if (stack == 0 || stack == numStacks)
			{
				break;
			}
		}
	}

	// Indexes for bottom triangles
	for (int slice = 1; slice < numSlices + 1; slice++)
	{
		if (slice == numSlices)
		{
			indexes.push_back(0);
			indexes.push_back(1);
			indexes.push_back(slice);

			continue;
		}

		indexes.push_back(0);
		indexes.push_back(slice + 1);
		indexes.push_back(slice);
	}

	for (int stack = 0; stack < numStacks - 2; stack++)
	{
		for (int slice = 0; slice < numSlices; slice++)
		{
			int currentVertexIndex = stack * numSlices + slice + 1;

			if (slice == numSlices - 1)
			{
				indexes.push_back(currentVertexIndex);
				indexes.push_back(currentVertexIndex + 1);
				indexes.push_back(currentVertexIndex + numSlices);

				indexes.push_back(currentVertexIndex);
				indexes.push_back(currentVertexIndex - numSlices + 1);
				indexes.push_back(currentVertexIndex + 1);
				continue;
			}

			indexes.push_back(currentVertexIndex);
			indexes.push_back(currentVertexIndex + numSlices + 1);
			indexes.push_back(currentVertexIndex + numSlices);

			indexes.push_back(currentVertexIndex);
			indexes.push_back(currentVertexIndex + 1);
			indexes.push_back(currentVertexIndex + numSlices + 1);
		}
	}


	// Indexes for top triangles
	for (int slice = 1; slice < numSlices + 1; slice++)
	{
		if (slice == 1)
		{
			indexes.push_back((int)verts.size() - 1);
			indexes.push_back((int)verts.size() - slice - 1);
			indexes.push_back((int)verts.size() - slice - numSlices);

			continue;
		}

		indexes.push_back((int)verts.size() - 1);
		indexes.push_back((int)verts.size() - slice - 1);
		indexes.push_back((int)verts.size() - slice);
	}
}

/*! \brief Adds vertexes and indexes for rendering an indexed sphere
*
* Pushes back vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the sphere.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the sphere should be appended
* \param indexes The list of indexes, passed by reference, to which the vertex indexes for the sphere should be appended
* \param center A Vec3 representing the center of the sphere
* \param radius The radius of the sphere
* \param color (Optional) The Rgba8 color that the sphere should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) (UNUSED) An AABB2 representing the texture UV coordinates to be applied to the sphere. Defaults to AABB2::ZERO_TO_ONE
* \param numSlices (Optional) The number of slices to be used when drawing the sphere. Defaults to 16
* \param numStacks (Optional) The number of stacks to be used when drawing the sphere. Defaults to 8
* \sa IndexBuffer
*
*/
void AddVertsForSphere3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& uvCoords, int numSlices, int numStacks)
{
	UNUSED(uvCoords);

	float degreeIncrementPerSlice = 360.f / (float)numSlices;

	for (int stack = 0; stack < numStacks + 1; stack++)
	{
		for (int slice = 0; slice < numSlices; slice++)
		{
			Vec3 vertexPosition = center + Vec3::MakeFromPolarDegrees((float)slice * degreeIncrementPerSlice, RangeMap((float)stack, 0.f, (float)numStacks, 90.f, -90.f), radius);
			float uCoord = RangeMap((float)slice, 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
			float vCoord = RangeMap((float)stack, 0.f, (float)numStacks, uvCoords.m_mins.y, uvCoords.m_maxs.y);
			Vec3 vertexNormal = (vertexPosition - center).GetNormalized();
			verts.push_back(Vertex_PCUTBN(vertexPosition, color, Vec2(uCoord, vCoord), Vec3::ZERO, Vec3::ZERO, vertexNormal));

			if (stack == 0 || stack == numStacks)
			{
				break;
			}
		}
	}

	// Indexes for bottom triangles
	for (int slice = 1; slice < numSlices + 1; slice++)
	{
		if (slice == numSlices)
		{
			indexes.push_back(0);
			indexes.push_back(1);
			indexes.push_back(slice);

			continue;
		}

		indexes.push_back(0);
		indexes.push_back(slice + 1);
		indexes.push_back(slice);
	}

	for (int stack = 0; stack < numStacks - 2; stack++)
	{
		for (int slice = 0; slice < numSlices; slice++)
		{
			int currentVertexIndex = stack * numSlices + slice + 1;

			if (slice == numSlices - 1)
			{
				indexes.push_back(currentVertexIndex);
				indexes.push_back(currentVertexIndex + 1);
				indexes.push_back(currentVertexIndex + numSlices);

				indexes.push_back(currentVertexIndex);
				indexes.push_back(currentVertexIndex - numSlices + 1);
				indexes.push_back(currentVertexIndex + 1);
				continue;
			}

			indexes.push_back(currentVertexIndex);
			indexes.push_back(currentVertexIndex + numSlices + 1);
			indexes.push_back(currentVertexIndex + numSlices);

			indexes.push_back(currentVertexIndex);
			indexes.push_back(currentVertexIndex + 1);
			indexes.push_back(currentVertexIndex + numSlices + 1);
		}
	}


	// Indexes for top triangles
	for (int slice = 1; slice < numSlices + 1; slice++)
	{
		if (slice == 1)
		{
			indexes.push_back((int)verts.size() - 1);
			indexes.push_back((int)verts.size() - slice - 1);
			indexes.push_back((int)verts.size() - slice - numSlices);

			continue;
		}

		indexes.push_back((int)verts.size() - 1);
		indexes.push_back((int)verts.size() - slice - 1);
		indexes.push_back((int)verts.size() - slice);
	}
}

/*! \brief Adds vertexes for rendering a cylinder
* 
* Pushes back vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the cylinder.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the cylinder should be appended
* \param start A Vec3 representing the center of the base of the cylinder
* \param end A Vec3 representing the center of the top of the cylinder
* \param radius The radius of the cylinder
* \param color (Optional) The Rgba8 color that the cylinder should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) (UNUSED) An AABB2 representing the texture UV coordinates to be applied to the cylinder. Defaults to AABB2::ZERO_TO_ONE
* \param numSlices (Optional) The number of slices to be used when drawing the cylinder. Defaults to 8
* 
*/
void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color, AABB2 const& uvCoords, int numSlices)
{
	UNUSED(uvCoords);

	std::vector<Vertex_PCU> cylinderVerts;

	float degreesIncrementPerSlice = 360.f / (float)numSlices;

	for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++)
	{
		Vec3 BL = Vec3::MakeFromCylindricalDegrees(1.f, sliceIndex * degreesIncrementPerSlice, 0.f);
		Vec3 BR = Vec3::MakeFromCylindricalDegrees(1.f, (float)(sliceIndex + 1) * degreesIncrementPerSlice, 0.f);
		Vec3 TL = Vec3::MakeFromCylindricalDegrees(1.f, sliceIndex * degreesIncrementPerSlice, 1.f);
		Vec3 TR = Vec3::MakeFromCylindricalDegrees(1.f, (float)(sliceIndex + 1) * degreesIncrementPerSlice, 1.f);

		float uMin = RangeMap((float)sliceIndex, 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
		float vMin = uvCoords.m_mins.y;
		float uMax = RangeMap((float)(sliceIndex + 1), 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
		float vMax = uvCoords.m_maxs.y;

		AddVertsForQuad3D(cylinderVerts, BL, BR, TR, TL, color, AABB2(Vec2(uMin, vMin), Vec2(uMax, vMax)));

		Vec2 uvBLDisc = Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(static_cast<float>(sliceIndex) * degreesIncrementPerSlice, 0.5f).GetRotated90Degrees();
		Vec2 uvBRDisc = Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(static_cast<float>(sliceIndex + 1) * degreesIncrementPerSlice, 0.5f).GetRotated90Degrees();

		uvBLDisc = uvCoords.GetPointAtUV(uvBLDisc);
		uvBRDisc = uvCoords.GetPointAtUV(uvBRDisc);

		cylinderVerts.push_back(Vertex_PCU(Vec3::ZERO, color, Vec2(0.5f, 0.5f)));
		cylinderVerts.push_back(Vertex_PCU(BR, color, uvBRDisc));
		cylinderVerts.push_back(Vertex_PCU(BL, color, uvBLDisc));

		cylinderVerts.push_back(Vertex_PCU(Vec3(0.f, 0.f, 1.f), color, Vec2(0.5f, 0.5f)));
		cylinderVerts.push_back(Vertex_PCU(TL, color, uvBLDisc));
		cylinderVerts.push_back(Vertex_PCU(TR, color, uvBRDisc));
	}

	Vec3 cylinderAxis = end - start;
	Vec3 cylinderLeft;
	Vec3 cylinderUp;

	if (CrossProduct3D(cylinderAxis, Vec3::EAST) == Vec3::ZERO)
	{
		cylinderLeft = Vec3::NORTH * radius;
	}
	else
	{
		cylinderLeft = CrossProduct3D(cylinderAxis, Vec3::EAST).GetNormalized() * radius;
	}

	cylinderUp = CrossProduct3D(cylinderAxis, cylinderLeft).GetNormalized() * radius;

	Mat44 cylinderTransformMatrix = Mat44(cylinderLeft, cylinderUp, cylinderAxis, start);
	TransformVertexArray3D(cylinderVerts, cylinderTransformMatrix);

	for (int vertexIndex = 0; vertexIndex < (int)(cylinderVerts.size()); vertexIndex++)
	{
		verts.push_back(cylinderVerts[vertexIndex]);
	}
}

/*! \brief Adds vertexes for rendering a cylinder
* 
* Pushes back vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the cylinder.
* \param verts The list of Vertex_PCUTBN objects, passed by reference, to which the vertexes for the cylinder should be appended
* \param start A Vec3 representing the center of the base of the cylinder
* \param end A Vec3 representing the center of the top of the cylinder
* \param radius The radius of the cylinder
* \param color (Optional) The Rgba8 color that the cylinder should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) (UNUSED) An AABB2 representing the texture UV coordinates to be applied to the cylinder. Defaults to AABB2::ZERO_TO_ONE
* \param numSlices (Optional) The number of slices to be used when drawing the cylinder. Defaults to 8
*
*/
void AddVertsForCylinder3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color, AABB2 const& uvCoords, int numSlices)
{
	UNUSED(uvCoords);

	std::vector<Vertex_PCUTBN> cylinderVerts;

	float degreesIncrementPerSlice = 360.f / (float)numSlices;

	for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++)
	{
		Vec3 BL = Vec3::MakeFromCylindricalDegrees(1.f, sliceIndex * degreesIncrementPerSlice, 0.f);
		Vec3 BR = Vec3::MakeFromCylindricalDegrees(1.f, (float)(sliceIndex + 1) * degreesIncrementPerSlice, 0.f);
		Vec3 TL = Vec3::MakeFromCylindricalDegrees(1.f, sliceIndex * degreesIncrementPerSlice, 1.f);
		Vec3 TR = Vec3::MakeFromCylindricalDegrees(1.f, (float)(sliceIndex + 1) * degreesIncrementPerSlice, 1.f);

		float uMin = RangeMap((float)sliceIndex, 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
		float vMin = uvCoords.m_mins.y;
		float uMax = RangeMap((float)(sliceIndex + 1), 0.f, (float)numSlices, uvCoords.m_mins.x, uvCoords.m_maxs.x);
		float vMax = uvCoords.m_maxs.y;

		AddVertsForQuad3D(cylinderVerts, BL, BR, TR, TL, color, AABB2(Vec2(uMin, vMin), Vec2(uMax, vMax)));

		Vec2 uvBLDisc = Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(static_cast<float>(sliceIndex) * degreesIncrementPerSlice, 0.5f).GetRotated90Degrees();
		Vec2 uvBRDisc = Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(static_cast<float>(sliceIndex + 1) * degreesIncrementPerSlice, 0.5f).GetRotated90Degrees();

		uvBLDisc = uvCoords.GetPointAtUV(uvBLDisc);
		uvBRDisc = uvCoords.GetPointAtUV(uvBRDisc);

		cylinderVerts.push_back(Vertex_PCUTBN(Vec3::ZERO, color, Vec2(0.5f, 0.5f), Vec3::ZERO, Vec3::ZERO, Vec3::GROUNDWARD));
		cylinderVerts.push_back(Vertex_PCUTBN(BR, color, uvBRDisc, Vec3::ZERO, Vec3::ZERO, Vec3::GROUNDWARD));
		cylinderVerts.push_back(Vertex_PCUTBN(BL, color, uvBLDisc, Vec3::ZERO, Vec3::ZERO, Vec3::GROUNDWARD));

		cylinderVerts.push_back(Vertex_PCUTBN(Vec3(0.f, 0.f, 1.f), color, Vec2(0.5f, 0.5f), Vec3::ZERO, Vec3::ZERO, Vec3::SKYWARD));
		cylinderVerts.push_back(Vertex_PCUTBN(TL, color, uvBLDisc, Vec3::ZERO, Vec3::ZERO, Vec3::SKYWARD));
		cylinderVerts.push_back(Vertex_PCUTBN(TR, color, uvBRDisc, Vec3::ZERO, Vec3::ZERO, Vec3::SKYWARD));
	}

	Vec3 cylinderAxis = end - start;
	Vec3 cylinderLeft;
	Vec3 cylinderUp;

	if (CrossProduct3D(cylinderAxis, Vec3::EAST) == Vec3::ZERO)
	{
		cylinderLeft = Vec3::NORTH * radius;
	}
	else
	{
		cylinderLeft = CrossProduct3D(cylinderAxis, Vec3::EAST).GetNormalized() * radius;
	}

	cylinderUp = CrossProduct3D(cylinderAxis, cylinderLeft).GetNormalized() * radius;

	Mat44 cylinderTransformMatrix = Mat44(cylinderLeft, cylinderUp, cylinderAxis, start);
	TransformVertexArray3D(cylinderVerts, cylinderTransformMatrix);

	for (int vertexIndex = 0; vertexIndex < (int)(cylinderVerts.size()); vertexIndex++)
	{
		verts.push_back(cylinderVerts[vertexIndex]);
	}
}

/*! \brief Adds vertexes for rendering a cone
*
* Pushes back vertexes to the end of the provided list. Game code may (and should) reserve vertexes but not construct placeholder instances in the list. This function does not support textures on the cone.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the cylinder should be appended
* \param baseCenter A Vec3 representing the center of the base of the cone
* \param apex A Vec3 representing the apex of the cone
* \param radius The radius of the cone
* \param color (Optional) The Rgba8 color that the cylinder should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) (UNUSED) An AABB2 representing the texture UV coordinates to be applied to the cylinder. Defaults to AABB2::ZERO_TO_ONE
* \param numSlices (Optional) The number of slices to be used when drawing the cylinder. Defaults to 8
*
*/
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, Vec3 const& baseCenter, Vec3 const& apex, float radius, Rgba8 const& color, AABB2 const& uvCoords, int numSlices)
{
	UNUSED(uvCoords);

	std::vector<Vertex_PCU> coneVerts;

	float degreesIncrementPerSlice = 360.f / (float)numSlices;
	for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++)
	{
		Vec3 BL = Vec3::MakeFromCylindricalDegrees(1.f, (float)sliceIndex * degreesIncrementPerSlice, 0.f);
		Vec3 BR = Vec3::MakeFromCylindricalDegrees(1.f, (float)(sliceIndex + 1) * degreesIncrementPerSlice, 0.f);

		coneVerts.push_back(Vertex_PCU(Vec3(0.f, 0.f, 1.f), color, Vec2::ZERO));
		coneVerts.push_back(Vertex_PCU(BL, color, Vec2::ZERO));
		coneVerts.push_back(Vertex_PCU(BR, color, Vec2::ZERO));
		
		coneVerts.push_back(Vertex_PCU(Vec3(0.f, 0.f, 0.f), color, Vec2::ZERO));
		coneVerts.push_back(Vertex_PCU(BR, color, Vec2::ZERO));
		coneVerts.push_back(Vertex_PCU(BL, color, Vec2::ZERO));
	}

	Vec3 coneAxis = apex - baseCenter;
	Vec3 coneLeft;
	Vec3 coneUp;

	if (CrossProduct3D(coneAxis, Vec3::EAST) == Vec3::ZERO)
	{
		coneLeft = Vec3::NORTH * radius;
	}
	else
	{
		coneLeft = CrossProduct3D(coneAxis, Vec3::EAST).GetNormalized() * radius;
	}

	coneUp = CrossProduct3D(coneAxis, coneLeft).GetNormalized() * radius;

	Mat44 coneTransformMatrix = Mat44(coneLeft, coneUp, coneAxis, baseCenter);
	TransformVertexArray3D(coneVerts, coneTransformMatrix);

	for (int vertexIndex = 0; vertexIndex < (int)(coneVerts.size()); vertexIndex++)
	{
		verts.push_back(coneVerts[vertexIndex]);
	}
}

/*! \brief Adds vertexes for rendering an arrow
* 
* An arrow is made up of a cylindrical line and a conical arrow. The radius for the arrow is three times the radius of the line passed, while the height of the cone forming the arrow is 0.01 times the length from the arrow tail to the arrow head, clamped to a minimum height of __ and a maximum height of __. This function does not support textures on the arrow.
* \param verts The list of Vertex_PCU objects, passed by reference, to which the vertexes for the arrow should be appended
* \param arrowTail A Vec3 representing the start of arrow
* \param arrowHead A Vec3 representing the end of the arrow (where the arrow points)
* \param radius The radius of the cylindrical line
* \param color (Optional) The Rgba8 color that the arrow should be drawn in. Defaults to Rgba8::WHITE
* \param uvCoords (Optional) (UNUSED) An AABB2 representing the texture UV coordinates to be applied to the arrow. Defaults to AABB2::ZERO_TO_ONE
* 
*/
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 const& arrowTailPosition, Vec3 const& arrowHeadPosition, float radius, Rgba8 const& color, AABB2 const& uvCoords)
{
	UNUSED(uvCoords);

	float arrowHeight = GetClamped((arrowHeadPosition - arrowTailPosition).GetLength() * 0.01f, radius * 3.f, radius * 5.f);
	Vec3 cylinderEnd = arrowHeadPosition - (arrowHeadPosition - arrowTailPosition).GetNormalized() * arrowHeight;
	AddVertsForCylinder3D(verts, arrowTailPosition, cylinderEnd, radius, color);
	AddVertsForCone3D(verts, cylinderEnd, arrowHeadPosition, radius * 3.f, color);
}

void AddVertsForLineSegment3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float width, Rgba8 const& color, AABB2 const& uvCoords)
{
	AddVertsForCylinder3D(verts, start, end, width, color, uvCoords, 4);
}

void AddVertsForGradientLineSegment3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float width, Rgba8 const& startColor, Rgba8 const& endColor, AABB2 const& uvCoords, int numSlices)
{
	std::vector<Vertex_PCU> cylinderVerts;

	float degreesIncrementPerSlice = 360.f / (float)numSlices;

	for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++)
	{
		Vec3 BL = Vec3::MakeFromCylindricalDegrees(1.f, sliceIndex * degreesIncrementPerSlice, 0.f);
		Vec3 BR = Vec3::MakeFromCylindricalDegrees(1.f, (float)(sliceIndex + 1) * degreesIncrementPerSlice, 0.f);
		Vec3 TL = Vec3::MakeFromCylindricalDegrees(1.f, sliceIndex * degreesIncrementPerSlice, 1.f);
		Vec3 TR = Vec3::MakeFromCylindricalDegrees(1.f, (float)(sliceIndex + 1) * degreesIncrementPerSlice, 1.f);

		Vertex_PCU bottomLeftVertex = Vertex_PCU(BL, startColor, uvCoords.m_mins);
		Vertex_PCU bottomRightVertex = Vertex_PCU(BR, startColor, Vec2(uvCoords.m_maxs.x, uvCoords.m_mins.y));
		Vertex_PCU topRightVertex = Vertex_PCU(TR, endColor, uvCoords.m_maxs);
		Vertex_PCU topLeftVertex = Vertex_PCU(TL, endColor, Vec2(uvCoords.m_mins.x, uvCoords.m_maxs.y));

		cylinderVerts.push_back(bottomLeftVertex);
		cylinderVerts.push_back(bottomRightVertex);
		cylinderVerts.push_back(topRightVertex);

		cylinderVerts.push_back(bottomLeftVertex);
		cylinderVerts.push_back(topRightVertex);
		cylinderVerts.push_back(topLeftVertex);

		Vec2 uvBLDisc = Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(static_cast<float>(sliceIndex) * degreesIncrementPerSlice, 0.5f).GetRotated90Degrees();
		Vec2 uvBRDisc = Vec2(0.5f, 0.5f) + Vec2::MakeFromPolarDegrees(static_cast<float>(sliceIndex + 1) * degreesIncrementPerSlice, 0.5f).GetRotated90Degrees();

		uvBLDisc = uvCoords.GetPointAtUV(uvBLDisc);
		uvBRDisc = uvCoords.GetPointAtUV(uvBRDisc);

		cylinderVerts.push_back(Vertex_PCU(Vec3::ZERO, startColor, Vec2(0.5f, 0.5f)));
		cylinderVerts.push_back(Vertex_PCU(BR, startColor, uvBRDisc));
		cylinderVerts.push_back(Vertex_PCU(BL, startColor, uvBLDisc));

		cylinderVerts.push_back(Vertex_PCU(Vec3(0.f, 0.f, 1.f), endColor, Vec2(0.5f, 0.5f)));
		cylinderVerts.push_back(Vertex_PCU(TL, endColor, uvBLDisc));
		cylinderVerts.push_back(Vertex_PCU(TR, endColor, uvBRDisc));
	}

	Vec3 cylinderAxis = end - start;
	Vec3 cylinderLeft;
	Vec3 cylinderUp;

	if (CrossProduct3D(cylinderAxis, Vec3::EAST) == Vec3::ZERO)
	{
		cylinderLeft = Vec3::NORTH * width;
	}
	else
	{
		cylinderLeft = CrossProduct3D(cylinderAxis, Vec3::EAST).GetNormalized() * width;
	}

	cylinderUp = CrossProduct3D(cylinderAxis, cylinderLeft).GetNormalized() * width;

	Mat44 cylinderTransformMatrix = Mat44(cylinderLeft, cylinderUp, cylinderAxis, start);
	TransformVertexArray3D(cylinderVerts, cylinderTransformMatrix);

	for (int vertexIndex = 0; vertexIndex < (int)(cylinderVerts.size()); vertexIndex++)
	{
		verts.push_back(cylinderVerts[vertexIndex]);
	}
}

void AddVertsForWireframePlane3(std::vector<Vertex_PCU>& verts, Plane3 const& plane)
{
	constexpr float lineHalfThickness = 0.01f;
	constexpr float line5HalfThickness = 0.02f;
	constexpr float line0HalfThickness = 0.04f;
	for (int y = -50; y <= 50; y++)
	{
		if (y == 0)
		{
			AddVertsForAABB3(verts, AABB3(Vec3(-50.f, (float)(y - line0HalfThickness), -line0HalfThickness), Vec3(50.f, (float)(y + line0HalfThickness), line0HalfThickness)), Rgba8::RED);
			continue;
		}

		if (y % 5 == 0)
		{
			AddVertsForAABB3(verts, AABB3(Vec3(-50.f, (float)(y - line5HalfThickness), -line5HalfThickness), Vec3(50.f, (float)(y + line5HalfThickness), line5HalfThickness)), Rgba8::RED);
			continue;
		}

		AddVertsForAABB3(verts, AABB3(Vec3(-50.f, (float)(y - lineHalfThickness), -lineHalfThickness), Vec3(50.f, (float)(y + lineHalfThickness), lineHalfThickness)), Rgba8::GRAY);
	}
	for (int x = -50; x <= 50; x++)
	{
		if (x == 0)
		{
			AddVertsForAABB3(verts, AABB3(Vec3((float)(x - line0HalfThickness), -50.f, -line0HalfThickness), Vec3((float)(x + line0HalfThickness), 50.f, line0HalfThickness)), Rgba8::GREEN);
			continue;
		}

		if (x % 5 == 0)
		{
			AddVertsForAABB3(verts, AABB3(Vec3((float)(x - line5HalfThickness), -50.f, -line5HalfThickness), Vec3((float)(x + line5HalfThickness), 50.f, line5HalfThickness)), Rgba8::GREEN);
			continue;
		}

		AddVertsForAABB3(verts, AABB3(Vec3((float)(x - lineHalfThickness), -50.f, -lineHalfThickness), Vec3((float)(x + lineHalfThickness), 50.f, lineHalfThickness)), Rgba8::GRAY);
	}

	Vec3 planeCenter = plane.GetCenter();
	Vec3 planeIBasis;
	Vec3 planeJBasis;
	if (CrossProduct3D(Vec3::SKYWARD, plane.m_normal) == Vec3::ZERO)
	{
		planeJBasis = Vec3::NORTH;
	}
	else
	{
		planeJBasis = CrossProduct3D(Vec3::SKYWARD, plane.m_normal).GetNormalized();
	}

	planeIBasis = CrossProduct3D(planeJBasis, plane.m_normal).GetNormalized();

	Mat44 planeTransformMatrix = Mat44(planeIBasis, planeJBasis, plane.m_normal, planeCenter);
	TransformVertexArray3D(verts, planeTransformMatrix);
}
