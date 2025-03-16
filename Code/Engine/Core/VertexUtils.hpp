#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

#include <vector>

struct AABB2;
struct OBB2;
struct OBB3;
struct Rgba8;
struct Vec2;
struct Vertex_PCU;
struct Vertex_PCUTBN;
struct Mat44;

/*! \file VertexUtils.hpp
*	\brief Contains helper functions for vertex operations and rendering geometry
*/

void TransformVertexArrayXY3D(int numVerts, Vertex_PCU* verts, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY);
void TransformVertexArrayXY3D(std::vector<Vertex_PCU>&, float uniformScale, float rotationDegreesAboutZ, Vec2 const& translationXY);
void TransformVertexArray3D(std::vector<Vertex_PCU>& verts, Mat44 const& transform);
void TransformVertexArray3D(std::vector<Vertex_PCUTBN>& verts, Mat44 const& transform);
AABB2 GetVertexBounds2D(std::vector<Vertex_PCU> const& verts);

void AddVertsForCapsule2D(std::vector<Vertex_PCU>& verts, Vec2 const& boneStart, Vec2 const& boneEnd, float radius, Rgba8 const& color);
void AddVertsForDisc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, Rgba8 const& color, Vec2 const& uvAtMins = Vec2::ZERO, Vec2 const& uvAtMaxs = Vec2::ONE, int numTris = 16);
void AddVertsForRing2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float thickness, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForArc2D(std::vector<Vertex_PCU>& verts, Vec2 const& center, float radius, float thickness, float startAngle, float endAngle, Rgba8 const& color = Rgba8::WHITE);
void AddVertsForOrientedSector2D(std::vector<Vertex_PCU>& verts, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius, Rgba8 const& color);
void AddVertsForDirectedSector2D(std::vector<Vertex_PCU>& verts, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius, Rgba8 const& color);
void AddVertsForAABB2(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, Rgba8 const& color, Vec2 const& uvAtMins = Vec2::ZERO, Vec2 const& uvAtMaxs = Vec2::ONE);
void AddVertsForOBB2(std::vector<Vertex_PCU>& verts, OBB2 const& box, Rgba8 const& color);
void AddVertsForLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& color);
void AddVertsForGradientLineSegment2D(std::vector<Vertex_PCU>& verts, Vec2 const& start, Vec2 const& end, float thickness, Rgba8 const& startColor, Rgba8 const& endColor);
void AddVertsForArrow2D(std::vector<Vertex_PCU>& verts, Vec2 const& tailPos, Vec2 const& tipPos, float arrowSize, float lineThickness, Rgba8 const& color);

void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForGradientQuad3D(std::vector<Vertex_PCU>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& BLColor = Rgba8::WHITE, Rgba8 const& BRColor = Rgba8::WHITE, Rgba8 const& TRColor = Rgba8::WHITE, Rgba8 const& TLColor = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topLeft, Vec3 const& topRight, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForRoundedQuad3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& bottomleft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForRing3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, float thickness, EulerAngles const& orientation, Rgba8 const& color = Rgba8::WHITE, int numTrapezoids = 64);
void AddVertsForDisc3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color, int numTris = 16);

void AddVertsForAABB3(std::vector<Vertex_PCU>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForAABB3(std::vector<Vertex_PCUTBN>& verts, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForAABB3(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords);
void AddVertsForAABB3(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, AABB3 const& bounds, Rgba8 const& color, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForOBB3(std::vector<Vertex_PCU>& verts, OBB3 const& box, Rgba8 const& color, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);

void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numStacks = 8, int numSlices = 16);
void AddVertsForSphere3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numStacks = 8, int numSlices = 16);
void AddVertsForSphere3D(std::vector<Vertex_PCU>& verts, std::vector<unsigned int>& indexes, Vec3 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numSlices = 16, int numStacks = 8);
void AddVertsForSphere3D(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indexes, Vec3 const& center, float radius, Rgba8 const& color, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numSlices = 16, int numStacks = 8);

void AddVertsForCylinder3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForCylinder3D(std::vector<Vertex_PCUTBN>& verts, Vec3 const& start, Vec3 const& end, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForCone3D(std::vector<Vertex_PCU>& verts, Vec3 const& baseCenter, Vec3 const& apex, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForArrow3D(std::vector<Vertex_PCU>& verts, Vec3 const& arrowTailPosition, Vec3 const& arrowHeadPosition, float radius, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForLineSegment3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float width, Rgba8 const& color = Rgba8::WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE);
void AddVertsForGradientLineSegment3D(std::vector<Vertex_PCU>& verts, Vec3 const& start, Vec3 const& end, float width, Rgba8 const& startColor = Rgba8::WHITE, Rgba8 const& endColor = Rgba8::TRANSPARENT_WHITE, AABB2 const& uvCoords = AABB2::ZERO_TO_ONE, int numSlices = 8);
void AddVertsForWireframePlane3(std::vector<Vertex_PCU>& verts, Plane3 const& plane);
