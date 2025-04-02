#include "MathUtils.hpp"
#include "MathUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"

#include <math.h>


bool AreFloatsMostlyEqual(float val1, float val2, float tolerance)
{
	return (fabsf(val1 - val2) < tolerance);
}

float GetClamped(float value, float minValue, float maxValue)
{
	if (value < minValue)
	{
		return minValue;
	}
	else if (value > maxValue)
	{
		return maxValue;
	}

	return value;
}

float GetClampedZeroToOne(float value)
{
	if (value < 0.f)
	{
		return 0;
	}
	else if (value > 1.f)
	{
		return 1.f;
	}

	return value;
}

float Interpolate(float start, float end, float fractionTowardEnd)
{
	return start + (end - start) * fractionTowardEnd;
}

Rgba8 const Interpolate(Rgba8 const& startColor, Rgba8 const& endColor, float fractionTowardEnd)
{
	unsigned char redByte = static_cast<unsigned char>(RoundDownToInt(Interpolate(static_cast<float>(startColor.r), static_cast<float>(endColor.r), fractionTowardEnd)));
	unsigned char greenByte = static_cast<unsigned char>(RoundDownToInt(Interpolate(static_cast<float>(startColor.g), static_cast<float>(endColor.g), fractionTowardEnd)));
	unsigned char blueByte = static_cast<unsigned char>(RoundDownToInt(Interpolate(static_cast<float>(startColor.b), static_cast<float>(endColor.b), fractionTowardEnd)));
	unsigned char alphaByte = static_cast<unsigned char>(RoundDownToInt(Interpolate(static_cast<float>(startColor.a), static_cast<float>(endColor.a), fractionTowardEnd)));

	return Rgba8(redByte, greenByte, blueByte, alphaByte);
}

float GetFractionWithinRange(float value, float rangeStart, float rangeEnd)
{
	return (value - rangeStart) / (rangeEnd - rangeStart);
}

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	float t = GetFractionWithinRange(inValue, inStart, inEnd);
	return Interpolate(outStart, outEnd, t);
}

float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	float rangeMappedValue = RangeMap(inValue, inStart, inEnd, outStart, outEnd);
	float outMin = outStart < outEnd ? outStart : outEnd;
	float outMax = outStart < outEnd ? outEnd : outStart;
	return GetClamped(rangeMappedValue, outMin, outMax);
}

int RoundDownToInt(float value)
{
	return (int)(floor(value));
}

float GetMin(float a, float b)
{
	return a <= b ? a : b;
}

float GetMin(std::vector<float> const& arr)
{
	float min = arr[0];
	for (int i = 1; i < (int)arr.size(); i++)
	{
		if (arr[i] < min)
		{
			min = arr[i];
		}
	}

	return min;
}

float GetMax(float a, float b)
{
	return a >= b ? a : b;
}

int GetMax(int a, int b)
{
	if (a > b)
	{
		return a;
	}

	return b;
}

int GetMax(int numElements, int* arr)
{
	int max = arr[0];
	for (int idx = 1; idx < numElements; idx++)
	{
		if (arr[idx] > max)
		{
			max = arr[idx];
		}
	}

	return max;
}

float CosDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return cosf(radians);
}

float SinDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return sinf(radians);
}

float TanDegrees(float degrees)
{
	float radians = ConvertDegreesToRadians(degrees);
	return tanf(radians);
}

float AcosDegrees(float value)
{
	float radians = acosf(value);
	return ConvertRadiansToDegrees(radians);
}

float AsinDegrees(float value)
{
	float radians = asinf(value);
	return ConvertRadiansToDegrees(radians);
}

float Atan2Degrees(float y, float x)
{
	float radians = atan2f(y, x);
	return ConvertRadiansToDegrees(radians);
}

float ConvertRadiansToDegrees(float radians)
{
	return radians * (180.f / PI);
}

float ConvertDegreesToRadians(float degrees)
{
	return degrees * (PI / 180.f);
}

float GetShortestAngularDispDegrees(float startDegrees, float endDegrees)
{
	while (startDegrees > 360.f)
	{
		startDegrees -= 360.f;
	}
	while (endDegrees > 360.f)
	{
		endDegrees -= 360.f;
	}

	float angularDisplacement = endDegrees - startDegrees;
	if (angularDisplacement > 180.f)
	{
		angularDisplacement -= 360.f;
	}
	else if (angularDisplacement < -180.f)
	{
		angularDisplacement += 360.f;
	}

	return angularDisplacement;
}

float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees)
{
	while (currentDegrees > 360.f)
	{
		currentDegrees -= 360.f;
	}
	while (goalDegrees > 360.f)
	{
		goalDegrees -= 360.f;
	}

	float turnedDegrees = currentDegrees;
	float shortestAngularDisplacement = GetShortestAngularDispDegrees(currentDegrees, goalDegrees);
	if (shortestAngularDisplacement > 0)
	{
		turnedDegrees += GetClamped(shortestAngularDisplacement, 0, maxDeltaDegrees);
	}
	else
	{
		turnedDegrees -= GetClamped(fabsf(shortestAngularDisplacement), 0, maxDeltaDegrees);
	}

	return turnedDegrees;
}

float GetAngleDegreesBetweenVectors2D(Vec2 const& vecA, Vec2 const& vecB)
{
	float dotProduct = DotProduct2D(vecA, vecB);
	
	float vecALength = vecA.GetLength();
	float vecBLength = vecB.GetLength();

	if (vecALength == 0 || vecBLength == 0)
	{
		return 0;
	}

	float radians = acosf(dotProduct / (vecALength * vecBLength));
	return ConvertRadiansToDegrees(radians);
}

float GetAngleDegreesBetweenVectors3D(Vec3 const& vecA, Vec3 const& vecB)
{
	float dotProduct = DotProduct3D(vecA, vecB);

	float vecALength = vecA.GetLength();
	float vecBLength = vecB.GetLength();

	if (vecALength == 0 || vecBLength == 0)
	{
		return 0;
	}

	float radians = acosf(dotProduct / (vecALength * vecBLength));
	return ConvertRadiansToDegrees(radians);
}

float DotProduct2D(Vec2 const& a, Vec2 const& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

float DotProduct3D(Vec3 const& a, Vec3 const& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float CrossProduct2D(Vec2 const& a, Vec2 const& b)
{
	return a.x * b.y - a.y * b.x;
}

Vec3 CrossProduct3D(Vec3 const& a, Vec3 const& b)
{
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float DotProduct4D(Vec4 const& a, Vec4 const& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

float GetDistance2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return sqrtf(
		((positionB.x - positionA.x) * (positionB.x - positionA.x)) +
		((positionB.y - positionA.y) * (positionB.y - positionA.y))
	);
}

float GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB)
{
	return (
		(positionB.x - positionA.x) * (positionB.x - positionA.x)) +
		((positionB.y - positionA.y) * (positionB.y - positionA.y)
	);
}

float GetDistance3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return sqrtf(
		((positionB.x - positionA.x) * (positionB.x - positionA.x)) +
		((positionB.y - positionA.y) * (positionB.y - positionA.y)) +
		((positionB.z - positionA.z) * (positionB.z - positionA.z))
	);
}

float GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (
		((positionB.x - positionA.x) * (positionB.x - positionA.x)) +
		((positionB.y - positionA.y) * (positionB.y - positionA.y)) +
		((positionB.z - positionA.z) * (positionB.z - positionA.z))
	);
}

float GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return sqrtf(
		((positionB.x - positionA.x) * (positionB.x - positionA.x)) +
		((positionB.y - positionA.y) * (positionB.y - positionA.y))
	);
}

float GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB)
{
	return (
		(positionB.x - positionA.x) * (positionB.x - positionA.x)) +
		((positionB.y - positionA.y) * (positionB.y - positionA.y)
	);
}

int GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB)
{
	return abs(pointA.x - pointB.x) + abs(pointA.y - pointB.y);
}

float GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	Vec2 normalToProjectOnto = vectorToProjectOnto.GetNormalized();
	return DotProduct2D(vectorToProject, normalToProjectOnto);
}

// #TODO Optimize this using (a.b / b * b) * b
// Eliminates taking a square root for normalizing
Vec2 const GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	Vec2 normalToProjectOnto = vectorToProjectOnto.GetNormalized();
	float projectedLength = DotProduct2D(vectorToProject, normalToProjectOnto);
	return projectedLength * normalToProjectOnto;
}

float GetProjectedLength3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto)
{
	Vec3 normalToProjectOnto = vectorToProjectOnto.GetNormalized();
	return DotProduct3D(vectorToProject, normalToProjectOnto);
}

Vec3 const GetProjectedOnto3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto)
{
	Vec3 normalToProjectOnto = vectorToProjectOnto.GetNormalized();
	float projectedLength = DotProduct3D(vectorToProject, normalToProjectOnto);
	return projectedLength * normalToProjectOnto;
}

bool IsPointInsideAABB2(Vec2 const& point, AABB2 const& box)
{
	return box.IsPointInside(point);
}

bool IsPointInsideOBB2(Vec2 const& point, OBB2 const& orientedBox)
{
	Vec2 displacementCenterToPoint = orientedBox.m_center - point;
	Vec2 jBasisNormal = orientedBox.m_iBasisNormal.GetRotated90Degrees();

	return (fabsf(GetProjectedLength2D(displacementCenterToPoint, orientedBox.m_iBasisNormal)) < orientedBox.m_halfDimensions.x &&
			fabsf(GetProjectedLength2D(displacementCenterToPoint, jBasisNormal)) < orientedBox.m_halfDimensions.y);
}

bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	Vec2 nearestPointOnLineSegment = GetNearestPointOnLineSegment2D(point, boneStart, boneEnd);
	return (GetDistance2D(point, nearestPointOnLineSegment) < radius);
}

bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius)
{
	return GetDistanceSquared2D(point, discCenter) < (discRadius * discRadius);
}

bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius)
{
	if (!IsPointInsideDisc2D(point, sectorTip, sectorRadius))
	{
		return false;
	}

	Vec2 sectorForwardNormal = Vec2::MakeFromPolarDegrees(sectorForwardDegrees);
	Vec2 displacementSectorTipToPoint = point - sectorTip;

	return GetAngleDegreesBetweenVectors2D(sectorForwardNormal, displacementSectorTipToPoint) < sectorApertureDegrees * 0.5f;
}

bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius)
{
	if (!IsPointInsideDisc2D(point, sectorTip, sectorRadius))
	{
		return false;
	}

	Vec2 displacementSectorTipToPoint = point - sectorTip;

	return GetAngleDegreesBetweenVectors2D(sectorForwardNormal, displacementSectorTipToPoint) < sectorApertureDegrees * 0.5f;
}

bool IsPointToLeftOfLine2D(Vec2 const& point, Vec2 const& lineStart, Vec2 const& lineEnd)
{
	Vec2 displacementStartToEnd = lineEnd - lineStart;
	Vec2 linePerpendicular = displacementStartToEnd.GetRotated90Degrees();
	Vec2 displacementLineStartToPoint = point - lineStart;
	float pointDistanceAlongPerpendicular = DotProduct2D(displacementLineStartToPoint, linePerpendicular);
	return (pointDistanceAlongPerpendicular > 0.f);
}

bool IsPointInsideConvexPoly2(Vec2 const& point, ConvexPoly2 const& convexPoly)
{
	std::vector<Vec2> vertexes = convexPoly.GetVertexes();

	for (int vertexIndex = 0; vertexIndex < vertexes.size() - 1; vertexIndex++)
	{
		if (!IsPointToLeftOfLine2D(point, vertexes[vertexIndex], vertexes[vertexIndex + 1]))
		{
			return false;
		}
	}

	return IsPointToLeftOfLine2D(point, vertexes[vertexes.size() - 1], vertexes[0]);
}

bool IsPointInsideConvexHull2(Vec2 const& point, ConvexHull2 const& convexHull)
{
	std::vector<Plane2> planes = convexHull.GetPlanes();

	for (int planeIndex = 0; planeIndex < planes.size(); planeIndex++)
	{
		if (!planes[planeIndex].IsPointBehind(point))
		{
			return false;
		}
	}

	return true;
}

bool IsPointInsideSphere3D(Vec3 const& point, Vec3 const& sphereCenter, float sphereRadius)
{
	return (GetDistanceSquared3D(point, sphereCenter) < sphereRadius * sphereRadius);
}

bool IsPointInsideAABB3(Vec3 const& point, AABB3 const& box)
{
	return box.IsPointInside(point);
}

bool IsPointInsideOBB3(Vec3 const& point, OBB3 const& orientedBox)
{
	Vec3 displacementCenterToPoint = orientedBox.m_center - point;

	return (fabsf(GetProjectedLength3D(displacementCenterToPoint, orientedBox.m_iBasis)) < orientedBox.m_halfDimensions.x &&
		fabsf(GetProjectedLength3D(displacementCenterToPoint, orientedBox.m_jBasis)) < orientedBox.m_halfDimensions.y &&
		fabsf(GetProjectedLength3D(displacementCenterToPoint, orientedBox.m_kBasis)) < orientedBox.m_halfDimensions.z);
}

bool IsPointInsideCylinder3D(Vec3 const& point, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius)
{
	Vec3 cylinderUp = cylinderTopCenter - cylinderBaseCenter;
	cylinderUp = cylinderUp.GetNormalized();
	Vec3 cylinderLeft;
	Vec3 cylinderForward;
	if (cylinderUp == Vec3::EAST)
	{
		cylinderLeft = Vec3::NORTH;
	}
	else
	{
		cylinderLeft = CrossProduct3D(cylinderUp, Vec3::EAST).GetNormalized();
	}
	cylinderForward = CrossProduct3D(cylinderLeft, cylinderUp).GetNormalized();

	Mat44 cylinderLocalToWorldMatrix = Mat44(cylinderForward, cylinderLeft, cylinderUp, cylinderBaseCenter);
	Mat44 worldToCylinderLocalMatrix = cylinderLocalToWorldMatrix.GetOrthonormalInverse();

	worldToCylinderLocalMatrix.TransformPosition3D(point);
	if (!IsPointInsideDisc2D(point.GetXY(), cylinderBaseCenter.GetXY(), cylinderRadius))
	{
		return false;
	}
	if (point.z >= cylinderTopCenter.z || point.z <= cylinderBaseCenter.z)
	{
		return false;
	}

	return true;
}

bool IsPointInsideCapsule3D(Vec3 const& point, Vec3 const& capsuleStart, Vec3 const& capsuleEnd, float capsuleRadius)
{
	Vec3 nearestPointOnLineSegment = GetNearestPointOnLineSegment3D(point, capsuleStart, capsuleEnd);
	return (GetDistance3D(point, nearestPointOnLineSegment) < capsuleRadius);
}

bool IsDiscCompletelyInsideDisc(Vec2 const& smallerDiscCenter, float smallerDiscRadius, Vec2 const& biggerDiscCenter, float biggerDiscRadius)
{
	return ((GetDistance2D(smallerDiscCenter, biggerDiscCenter) + smallerDiscRadius) <= biggerDiscRadius);
}

bool DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB)
{
	return (GetDistanceSquared2D(centerA, centerB) < (radiusA + radiusB) * (radiusA + radiusB));
}

bool DoAABB2Overlap(AABB2 const& boxA, AABB2 const& boxB)
{
	if (boxA.m_mins.x >= boxB.m_maxs.x)
	{
		return false;
	}
	if (boxA.m_maxs.x <= boxB.m_mins.x)
	{
		return false;
	}
	if (boxA.m_mins.y >= boxB.m_maxs.y)
	{
		return false;
	}
	if (boxA.m_maxs.y <= boxB.m_mins.y)
	{
		return false;
	}

	return true;
}

bool DoDiscAndAABB2Overlap(Vec2 const& discCenter, float discRadius, AABB2 const& box)
{
	Vec2 nearestPoint = GetNearestPointOnAABB2(discCenter, box);
	return IsPointInsideDisc2D(nearestPoint, discCenter, discRadius);
}

bool DoAABB2AndDiscOverlap(AABB2 const& box, Vec2 const& discCenter, float discRadius)
{
	Vec2 nearestPoint = GetNearestPointOnAABB2(discCenter, box);
	return IsPointInsideDisc2D(nearestPoint, discCenter, discRadius);
}

bool DoDiscAndCapsuleOverlap(Vec2 const& discCenter, float discRadius, Vec2 const& capsuleStart, Vec2 const& capsuleEnd, float capsuleRadius)
{
	Vec2 nearestPoint = GetNearestPointOnCapsule2D(discCenter, capsuleStart, capsuleEnd, capsuleRadius);
	return IsPointInsideDisc2D(nearestPoint, discCenter, discRadius);
}

bool DoCapsuleAndDiscOverlap(Vec2 const& capsuleStart, Vec2 const& capsuleEnd, float capsuleRadius, Vec2 const& discCenter, float discRadius)
{
	return DoDiscAndCapsuleOverlap(discCenter, discRadius, capsuleStart, capsuleEnd, capsuleRadius);
}

bool DoDiscAndOBB2Overlap(Vec2 const& discCenter, float discRadius, OBB2 const& box)
{
	Vec2 nearestPoint = GetNearestPointOnOBB2(discCenter, box);
	return IsPointInsideDisc2D(nearestPoint, discCenter, discRadius);
}

bool DoOBB2AndDiscOverlap(OBB2 const& box, Vec2 const& discCenter, float discRadius)
{
	return DoDiscAndOBB2Overlap(discCenter, discRadius, box);
}

bool DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB)
{
	return (GetDistance3D(centerA, centerB) < (radiusA + radiusB));
}

bool DoAABB3Overlap(AABB3 const& boxA, AABB3 const& boxB)
{
	if (boxA.m_mins.x >= boxB.m_maxs.x)
	{
		return false;
	}
	if (boxA.m_maxs.x <= boxB.m_mins.x)
	{
		return false;
	}
	if (boxA.m_mins.y >= boxB.m_maxs.y)
	{
		return false;
	}
	if (boxA.m_maxs.y <= boxB.m_mins.y)
	{
		return false;
	}
	if (boxA.m_mins.z >= boxB.m_maxs.z)
	{
		return false;
	}
	if (boxA.m_maxs.z <= boxB.m_mins.z)
	{
		return false;
	}

	return true;
}

bool DoSphereAndAABB3Overlap(Vec3 const& sphereCenter, float radius, AABB3 const& box)
{
	Vec3 nearestPoint = GetNearestPointOnAABB3(sphereCenter, box);
	return IsPointInsideSphere3D(nearestPoint, sphereCenter, radius);
}

bool DoAABB3AndSphereOverlap(AABB3 const& box, Vec3 const& sphereCenter, float radius)
{
	Vec3 nearestPoint = GetNearestPointOnAABB3(sphereCenter, box);
	return IsPointInsideSphere3D(nearestPoint, sphereCenter, radius);
}

bool DoCylinderAndSphereOverlap(Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius, Vec3 const& sphereCenter, float sphereRadius)
{
	Vec3 nearestPoint = GetNearestPointOnCylinder3D(sphereCenter, cylinderBaseCenter, cylinderTopCenter, cylinderRadius);
	return IsPointInsideSphere3D(nearestPoint, sphereCenter, sphereRadius);
}

bool DoSphereAndCylinderOverlap(Vec3 const& sphereCenter, float sphereRadius, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius)
{
	Vec3 nearestPoint = GetNearestPointOnCylinder3D(sphereCenter, cylinderBaseCenter, cylinderTopCenter, cylinderRadius);
	return IsPointInsideSphere3D(nearestPoint, sphereCenter, sphereRadius);
}

bool DoAABB3AndZCylinderOverlap(AABB3 const& box, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius)
{
	if (!DoDiscAndAABB2Overlap(cylinderBaseCenter.GetXY(), cylinderRadius, AABB2(box.m_mins.GetXY(), box.m_maxs.GetXY())))
	{
		return false;
	}
	FloatRange cylinderZRange(cylinderBaseCenter.z, cylinderTopCenter.z);
	FloatRange boxZRange(box.m_mins.z, box.m_maxs.z);
	if (!cylinderZRange.IsOverlappingWith(boxZRange))
	{
		return false;
	}

	return true;
}

bool DoZCylinderAndAABB3Overlap(Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius, AABB3 const& box)
{
	if (!DoDiscAndAABB2Overlap(cylinderBaseCenter.GetXY(), cylinderRadius, AABB2(box.m_mins.GetXY(), box.m_maxs.GetXY())))
	{
		return false;
	}
	FloatRange cylinderZRange(cylinderBaseCenter.z, cylinderTopCenter.z);
	FloatRange boxZRange(box.m_mins.z, box.m_maxs.z);
	if (!cylinderZRange.IsOverlappingWith(boxZRange))
	{
		return false;
	}

	return true;
}

bool DoZCylindersOverlap(Vec3 const& cylinderABaseCenter, Vec3 const& cylinderATopCenter, float cylinderARadius, Vec3 const& cylinderBBaseCenter, Vec3 const& cylinderBTopCenter, float cylinderBRadius)
{
	if (!DoDiscsOverlap(cylinderABaseCenter.GetXY(), cylinderARadius, cylinderBBaseCenter.GetXY(), cylinderBRadius))
	{
		return false;
	}
	FloatRange cylinderAZRange(cylinderABaseCenter.z, cylinderATopCenter.z);
	FloatRange cylinderBZRange(cylinderBBaseCenter.z, cylinderBTopCenter.z);
	if (!cylinderAZRange.IsOverlappingWith(cylinderBZRange))
	{
		return false;
	}

	return true;
}

bool DoOBB3AndPlane3Overlap(OBB3 const& orientedBox, Plane3 const& plane)
{
	Vec3 obbCornerPoints[8];
	orientedBox.GetCornerPoints(obbCornerPoints);
	bool foundPointInFront = false;
	bool foundPointBehind = false;
	for (int cornerPointIndex = 0; cornerPointIndex < 8; cornerPointIndex++)
	{
		if (plane.IsPointInFront(obbCornerPoints[cornerPointIndex]))
		{
			foundPointInFront = true;
		}
		else if (plane.IsPointBehind(obbCornerPoints[cornerPointIndex]))
		{
			foundPointBehind = true;
		}
	}

	return foundPointInFront && foundPointBehind;
}

bool DoPlane3AndOBB3Overlap(Plane3 const& plane, OBB3 const& orientedBox)
{
	return DoOBB3AndPlane3Overlap(orientedBox, plane);
}

bool DoSphereAndPlane3Overlap(Vec3 const& sphereCenter, float sphereRadius, Plane3 const& plane)
{
	float sphereAltitude = fabsf(GetProjectedLength3D(sphereCenter, plane.m_normal) - plane.m_distanceFromOriginAlongNormal);
	return sphereAltitude < sphereRadius;
}

bool DoPlane3AndSphereOverlap(Plane3 const& plane, Vec3 const& sphereCenter, float sphereRadius)
{
	return DoSphereAndPlane3Overlap(sphereCenter, sphereRadius, plane);
}

bool DoOBB3AndSphereOverlap(OBB3 const& orientedBox, Vec3 const& sphereCenter, float sphereRadius)
{
	Vec3 nearestPoint = GetNearestPointOnOBB3(sphereCenter, orientedBox);
	return IsPointInsideSphere3D(nearestPoint, sphereCenter, sphereRadius);
}

bool DoSphereAndOBB3Overlap(Vec3 const& sphereCenter, float sphereRadius, OBB3 const& orientedBox)
{
	return DoOBB3AndSphereOverlap(orientedBox, sphereCenter, sphereRadius);
}

bool DoAABB3AndPlane3Overlap(AABB3 const& box, Plane3 const& plane)
{
	Vec3 obbCornerPoints[8];
	box.GetCornerPoints(obbCornerPoints);
	bool foundPointInFront = false;
	bool foundPointBehind = false;
	for (int cornerPointIndex = 0; cornerPointIndex < 8; cornerPointIndex++)
	{
		if (plane.IsPointInFront(obbCornerPoints[cornerPointIndex]))
		{
			foundPointInFront = true;
		}
		else if (plane.IsPointBehind(obbCornerPoints[cornerPointIndex]))
		{
			foundPointBehind = true;
		}
	}

	return foundPointInFront && foundPointBehind;
}

bool DoPlane3AndAABB3Overlap(Plane3 const& plane, AABB3 const& box)
{
	return DoAABB3AndPlane3Overlap(box, plane);
}

bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedPoint)
{
	if (!IsPointInsideDisc2D(fixedPoint, mobileDiscCenter, mobileDiscRadius))
	{
		return false;
	}

	float pushDistance = mobileDiscRadius - GetDistance2D(fixedPoint, mobileDiscCenter);
	Vec2 pushDirection = (mobileDiscCenter - fixedPoint).GetNormalized();

	mobileDiscCenter += pushDirection * pushDistance;

	return true;
}

bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius)
{
	if (!DoDiscsOverlap(mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius))
	{
		return false;
	}

	Vec2 pushDirection = (mobileDiscCenter - fixedDiscCenter).GetNormalized();
	float pushDistance = (mobileDiscRadius + fixedDiscRadius) - GetDistance2D(mobileDiscCenter, fixedDiscCenter);

	mobileDiscCenter += pushDirection * pushDistance;

	return true;
}

bool PushDiscIntoFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius)
{
	if (IsDiscCompletelyInsideDisc(mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius))
	{
		return false;
	}

	Vec2 pushDirection = (fixedDiscCenter - mobileDiscCenter).GetNormalized();
	float pushDistance = GetDistance2D(mobileDiscCenter, fixedDiscCenter) + mobileDiscRadius - fixedDiscRadius;

	mobileDiscCenter += pushDirection * pushDistance;

	return true;
}

bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius)
{
	if (!DoDiscsOverlap(aCenter, aRadius, bCenter, bRadius))
	{
		return false;
	}

	Vec2 pushDirectionA = (aCenter - bCenter).GetNormalized();
	Vec2 pushDirectionB = -pushDirectionA;

	float pushDistance = (aRadius + bRadius) - GetDistance2D(aCenter, bCenter);
	pushDistance *= 0.5f;

	aCenter += pushDirectionA * pushDistance;
	bCenter += pushDirectionB * pushDistance;

	return true;
}

bool PushDiscOutOfFixedAABB2(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox)
{
	Vec2 nearestPointOnBox = fixedBox.GetNearestPoint(mobileDiscCenter);
	return PushDiscOutOfFixedPoint2D(mobileDiscCenter, discRadius, nearestPointOnBox);
}

bool PushDiscOutOfFixedOBB2(Vec2& mobileDiscCenter, float mobileDiscRadius, OBB2 const& fixedOrientedBox)
{
	Vec2 nearestPointOnOrientedBox = GetNearestPointOnOBB2(mobileDiscCenter, fixedOrientedBox);
	return PushDiscOutOfFixedPoint2D(mobileDiscCenter, mobileDiscRadius, nearestPointOnOrientedBox);
}

bool PushDiscOutOfFixedCapsule2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedCapsuleStart, Vec2 const& fixedCapsuleEnd, float capsuleRadius)
{
	Vec2 nearestPointOnCapsule = GetNearestPointOnCapsule2D(mobileDiscCenter, fixedCapsuleStart, fixedCapsuleEnd, capsuleRadius);
	return PushDiscOutOfFixedPoint2D(mobileDiscCenter, mobileDiscRadius, nearestPointOnCapsule);
}

bool PushSphereOutOfFixedPoint3D(Vec3& mobileSphereCenter, float mobileSphereRadius, Vec3 const& fixedPoint)
{
	if (!IsPointInsideSphere3D(fixedPoint, mobileSphereCenter, mobileSphereRadius))
	{
		return false;
	}

	float pushDistance = mobileSphereRadius - GetDistance3D(fixedPoint, mobileSphereCenter);
	Vec3 pushDirection = (mobileSphereCenter - fixedPoint).GetNormalized();

	mobileSphereCenter += pushDirection * pushDistance;

	return true;
}

bool PushSphereOutOfFixedSphere3D(Vec3 mobileSphereCenter, float mobileSphereRadius, Vec3 const& fixedSphereCenter, float fixedSphereRadius)
{
	if (!DoSpheresOverlap(mobileSphereCenter, mobileSphereRadius, fixedSphereCenter, fixedSphereRadius))
	{
		return false;
	}

	Vec3 pushDirection = (mobileSphereCenter - fixedSphereCenter).GetNormalized();
	float pushDistance = (mobileSphereRadius + fixedSphereRadius) - GetDistance3D(mobileSphereCenter, fixedSphereCenter);

	mobileSphereCenter += pushDirection * pushDistance;

	return true;
}

bool PushSpheresOutOfEachOther3D(Vec3& aCenter, float aRadius, Vec3& bCenter, float bRadius)
{
	if (!DoSpheresOverlap(aCenter, aRadius, bCenter, bRadius))
	{
		return false;
	}

	Vec3 pushDirectionA = (aCenter - bCenter).GetNormalized();
	Vec3 pushDirectionB = -pushDirectionA;

	float pushDistance = (aRadius + bRadius) - GetDistance3D(aCenter, bCenter);
	pushDistance *= 0.5f;

	aCenter += pushDirectionA * pushDistance;
	bCenter += pushDirectionB * pushDistance;

	return true;
}

bool PushZCylinderOutOfFixedAABB3(Vec3& cylinderBaseCenter, Vec3& cylinderTopCenter, float cylinderRadius, AABB3 const& box)
{
	AABB2 topViewBox2D(box.m_mins.GetXY(), box.m_maxs.GetXY());
	if (!DoDiscAndAABB2Overlap(cylinderBaseCenter.GetXY(), cylinderRadius, topViewBox2D))
	{
		return false;
	}

	FloatRange cylinderZRange(cylinderBaseCenter.z, cylinderTopCenter.z);
	FloatRange boxZRange(box.m_mins.z, box.m_maxs.z);

	if (!cylinderZRange.IsOverlappingWith(boxZRange))
	{
		return false;
	}

	Vec2 nearestPointOnTopViewBox2D = topViewBox2D.GetNearestPoint(cylinderBaseCenter.GetXY());
	float topViewPushDistance = cylinderRadius - GetDistance2D(nearestPointOnTopViewBox2D, cylinderBaseCenter.GetXY());

	float downwardPushDistance = fabsf(cylinderZRange.m_max - boxZRange.m_min);
	float upwardPushDistance = fabsf(cylinderZRange.m_min - boxZRange.m_max);
	float verticalPushDistance = upwardPushDistance;
	Vec3 verticalPushDirection = Vec3::SKYWARD;
	if (upwardPushDistance > downwardPushDistance)
	{
		verticalPushDistance = downwardPushDistance;
		verticalPushDirection = Vec3::GROUNDWARD;
	}


	Vec3 horizontalPushDir = (cylinderBaseCenter.GetXY() - nearestPointOnTopViewBox2D).ToVec3().GetNormalized();
	if (verticalPushDistance < topViewPushDistance)
	{
		cylinderBaseCenter += verticalPushDistance * verticalPushDirection;
		cylinderTopCenter += verticalPushDistance * verticalPushDirection;

		return true;
	}

	cylinderBaseCenter += horizontalPushDir * topViewPushDistance;
	cylinderTopCenter += horizontalPushDir * topViewPushDistance;

	return true;
}

bool PushAABB3OutOfFixedZCylinder(AABB3& box, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius)
{
	AABB2 topViewBox2D(box.m_mins.GetXY(), box.m_maxs.GetXY());
	if (!DoDiscAndAABB2Overlap(cylinderBaseCenter.GetXY(), cylinderRadius, topViewBox2D))
	{
		return false;
	}

	FloatRange cylinderZRange(cylinderBaseCenter.z, cylinderTopCenter.z);
	FloatRange boxZRange(box.m_mins.z, box.m_maxs.z);

	if (!cylinderZRange.IsOverlappingWith(boxZRange))
	{
		return false;
	}

	Vec2 nearestPointOnTopViewBox2D = topViewBox2D.GetNearestPoint(cylinderBaseCenter.GetXY());
	float topViewPushDistance = cylinderRadius - GetDistance2D(nearestPointOnTopViewBox2D, cylinderBaseCenter.GetXY());

	float downwardPushDistance = fabsf(cylinderZRange.m_max - boxZRange.m_min);
	float upwardPushDistance = fabsf(cylinderZRange.m_min - boxZRange.m_max);
	float verticalPushDistance = upwardPushDistance;
	Vec3 verticalPushDirection = Vec3::SKYWARD;
	if (upwardPushDistance > downwardPushDistance)
	{
		verticalPushDistance = downwardPushDistance;
		verticalPushDirection = Vec3::GROUNDWARD;
	}


	Vec3 horizontalPushDir = (cylinderBaseCenter.GetXY() - nearestPointOnTopViewBox2D).ToVec3().GetNormalized();
	if (verticalPushDistance < topViewPushDistance)
	{
		box.Translate(-verticalPushDirection * verticalPushDistance);

		return true;
	}

	box.Translate(-horizontalPushDir * topViewPushDistance);

	return true;
}

bool PushSphereOutOfFixedAABB3(Vec3& sphereCenter, float sphereRadius, AABB3 const& fixedBox)
{
	Vec3 const& nearestPoint = fixedBox.GetNearestPoint(sphereCenter);
	return PushSphereOutOfFixedPoint3D(sphereCenter, sphereRadius, nearestPoint);
}

bool PushSphereOutOfFixedOBB3(Vec3& sphereCenter, float sphereRadius, OBB3 const& fixedOrientedBox)
{
	Vec3 const& nearestPoint = GetNearestPointOnOBB3(sphereCenter, fixedOrientedBox);
	return PushSphereOutOfFixedPoint3D(sphereCenter, sphereRadius, nearestPoint);
}

bool BounceDiscsOffEachOther2D(Vec2& aCenter, float aRadius, Vec2& aVelocity, float aElasticity, Vec2& bCenter, float bRadius, Vec2& bVelocity, float bElasticity)
{
	if (!DoDiscsOverlap(aCenter, aRadius, bCenter, bRadius))
	{
		return false;
	}

	Vec2 directionBToA = (aCenter - bCenter).GetNormalized();
	Vec2 directionAToB = -directionBToA;
	Vec2 aNormalVelocity = GetProjectedOnto2D(aVelocity, directionAToB);
	Vec2 aTangentVelocity = aVelocity - aNormalVelocity;
	Vec2 bNormalVelocity = GetProjectedOnto2D(bVelocity, directionBToA);
	Vec2 bTangentVelocity = bVelocity - bNormalVelocity;
	float collisionElasticity = aElasticity * bElasticity;

	PushDiscsOutOfEachOther2D(aCenter, aRadius, bCenter, bRadius);

	if (AreVelocitiesDiverging2D(aVelocity, bVelocity, directionAToB))
	{
		return true;
	}

	Vec2 aFinalNormalVelocity = bNormalVelocity * collisionElasticity;
	Vec2 bFinalNormalVelocity = aNormalVelocity * collisionElasticity;
	aVelocity = aTangentVelocity + aFinalNormalVelocity;
	bVelocity = bTangentVelocity + bFinalNormalVelocity;

	return true;
}

bool BounceDiscOffFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, Vec2 const& fixedDiscCenter, float fixedDiscRadius, float fixedDiscElasticity)
{
	if (!DoDiscsOverlap(mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius))
	{
		return false;
	}

	Vec2 directionMobileToFixed = (fixedDiscCenter - mobileDiscCenter).GetNormalized();
	Vec2 mobileDiscNormalVelocity = GetProjectedOnto2D(mobileDiscVelocity, directionMobileToFixed);
	Vec2 mobileDiscTangentVelocity = mobileDiscVelocity - mobileDiscNormalVelocity;
	float collisionElasticity = mobileDiscElasticity * fixedDiscElasticity;

	PushDiscOutOfFixedDisc2D(mobileDiscCenter, mobileDiscRadius, fixedDiscCenter, fixedDiscRadius);

	if (AreVelocitiesDiverging2D(mobileDiscVelocity, Vec2::ZERO, directionMobileToFixed))
	{
		return true;
	}

	mobileDiscNormalVelocity = -mobileDiscNormalVelocity * collisionElasticity;
	mobileDiscVelocity = mobileDiscTangentVelocity + mobileDiscNormalVelocity;

	return true;
}

bool BounceDiscOffFixedOBB2(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, OBB2 const& fixedBox, float boxElasticity)
{
	if (!DoDiscAndOBB2Overlap(mobileDiscCenter, mobileDiscRadius, fixedBox))
	{
		return false;
	}

	Vec2 nearestPointOnBox = GetNearestPointOnOBB2(mobileDiscCenter, fixedBox);
	Vec2 directionDiscToPoint = (nearestPointOnBox - mobileDiscCenter).GetNormalized();
	Vec2 mobileDiscNormalVelocity = GetProjectedOnto2D(mobileDiscVelocity, directionDiscToPoint);
	Vec2 mobileDiscTangentVelocity = mobileDiscVelocity - mobileDiscNormalVelocity;
	float collisionElasticity = mobileDiscElasticity * boxElasticity;

	PushDiscOutOfFixedOBB2(mobileDiscCenter, mobileDiscRadius, fixedBox);

	if (AreVelocitiesDiverging2D(mobileDiscVelocity, Vec2::ZERO, directionDiscToPoint))
	{
		return true;
	}

	mobileDiscNormalVelocity = -mobileDiscNormalVelocity * collisionElasticity;
	mobileDiscVelocity = mobileDiscTangentVelocity + mobileDiscNormalVelocity;

	return true;
}

bool BounceDiscOffFixedAABB2(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, AABB2 const& fixedBox, float boxElasticity)
{
	if (!DoDiscAndAABB2Overlap(mobileDiscCenter, mobileDiscRadius, fixedBox))
	{
		return false;
	}

	Vec2 nearestPointOnBox = GetNearestPointOnAABB2(mobileDiscCenter, fixedBox);
	Vec2 directionDiscToPoint = (nearestPointOnBox - mobileDiscCenter).GetNormalized();
	Vec2 mobileDiscNormalVelocity = GetProjectedOnto2D(mobileDiscVelocity, directionDiscToPoint);
	Vec2 mobileDiscTangentVelocity = mobileDiscVelocity - mobileDiscNormalVelocity;
	float collisionElasticity = mobileDiscElasticity * boxElasticity;

	PushDiscOutOfFixedAABB2(mobileDiscCenter, mobileDiscRadius, fixedBox);

	if (AreVelocitiesDiverging2D(mobileDiscVelocity, Vec2::ZERO, directionDiscToPoint))
	{
		return true;
	}

	mobileDiscNormalVelocity = -mobileDiscNormalVelocity * collisionElasticity;
	mobileDiscVelocity = mobileDiscTangentVelocity + mobileDiscNormalVelocity;

	return true;
}

bool BounceDiscOffFixedCapsule2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, Vec2 const& fixedCapsuleStart, Vec2 const& fixedCapsuleEnd, float fixedCapsuleRadius, float capsuleElasticity)
{
	if (!DoDiscAndCapsuleOverlap(mobileDiscCenter, mobileDiscRadius, fixedCapsuleStart, fixedCapsuleEnd, fixedCapsuleRadius))
	{
		return false;
	}

	Vec2 nearestPointOnCapsule = GetNearestPointOnCapsule2D(mobileDiscCenter, fixedCapsuleStart, fixedCapsuleEnd, fixedCapsuleRadius);
	Vec2 directionDiscToPoint = (nearestPointOnCapsule - mobileDiscCenter).GetNormalized();
	Vec2 mobileDiscNormalVelocity = GetProjectedOnto2D(mobileDiscVelocity, directionDiscToPoint);
	Vec2 mobileDiscTangentVelocity = mobileDiscVelocity - mobileDiscNormalVelocity;
	float collisionElasticity = mobileDiscElasticity * capsuleElasticity;

	PushDiscOutOfFixedCapsule2D(mobileDiscCenter, mobileDiscRadius, fixedCapsuleStart, fixedCapsuleEnd, fixedCapsuleRadius);

	if (AreVelocitiesDiverging2D(mobileDiscVelocity, Vec2::ZERO, directionDiscToPoint))
	{
		return true;
	}

	mobileDiscNormalVelocity = -mobileDiscNormalVelocity * collisionElasticity;
	mobileDiscVelocity = mobileDiscTangentVelocity + mobileDiscNormalVelocity;

	return true;
}

Vec2 const GetNearestPointOnDisc2D(Vec2 const& referencePosition, Vec2 const& discCenter, float discRadius)
{
	Vec2 displacementCenterToPoint = (referencePosition - discCenter);
	return discCenter + displacementCenterToPoint.GetClamped(discRadius);
}

Vec2 const GetNearestPointOnAABB2(Vec2 const& referencePosition, AABB2 const& box)
{
	return box.GetNearestPoint(referencePosition);
}

Vec2 const GetNearestPointOnOBB2(Vec2 const& referencePosition, OBB2 const& orientedBox)
{
	Vec2 const& iBasisNormal = orientedBox.m_iBasisNormal;
	Vec2 jBasisNormal = iBasisNormal.GetRotated90Degrees();

	Vec2 referencePointInOBBLocalSpace = orientedBox.GetLocalPosForWorldPos(referencePosition);
	AABB2 OBB2InLocalSpace(-orientedBox.m_halfDimensions, orientedBox.m_halfDimensions);

	Vec2 nearestPointInOBBLocalSpace = GetNearestPointOnAABB2(referencePointInOBBLocalSpace, OBB2InLocalSpace);

	Vec2 nearestPointInWorldSpace = orientedBox.GetWorldPosForLocalPos(nearestPointInOBBLocalSpace);

	return nearestPointInWorldSpace;
}

Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, Vec2 const& linePointA, Vec2 const& linePointB)
{
	Vec2 displacementAToB = linePointB - linePointA;
	Vec2 displacementAToPoint = referencePosition - linePointA;
	return linePointA + GetProjectedOnto2D(displacementAToPoint, displacementAToB);
}

Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, Vec2 const& lineSegmentStart, Vec2 const& lineSegmentEnd)
{
	Vec2 displacementStartToEnd = lineSegmentEnd - lineSegmentStart;
	Vec2 displacementStartToPoint = referencePosition - lineSegmentStart;

	if (DotProduct2D(displacementStartToEnd, displacementStartToPoint) <= 0)
	{
		return lineSegmentStart;
	}


	Vec2 displacementEndToPoint = referencePosition - lineSegmentEnd;
	
	if (DotProduct2D(displacementStartToEnd, displacementEndToPoint) >= 0)
	{
		return lineSegmentEnd;
	}

	return lineSegmentStart + GetProjectedOnto2D(displacementStartToPoint, displacementStartToEnd);
}

Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	if (IsPointInsideCapsule2D(referencePosition, boneStart, boneEnd, radius))
	{
		return referencePosition;
	}

	Vec2 nearestPointOnLineSegment = GetNearestPointOnLineSegment2D(referencePosition, boneStart, boneEnd);
	Vec2 dispNearestPointOnLineSegToRefPosition = referencePosition - nearestPointOnLineSegment;
	return nearestPointOnLineSegment + (dispNearestPointOnLineSegToRefPosition.GetNormalized() * radius);
}

Vec2 const GetNearestPointOnOrientedSector2D(Vec2 const& referencePosition, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius)
{
	if (IsPointInsideOrientedSector2D(referencePosition, sectorTip, sectorForwardDegrees, sectorApertureDegrees, sectorRadius))
	{
		return referencePosition;
	}

	Vec2 displacementSectorTipToPoint = referencePosition - sectorTip;
	float rotationDegreesSectorForwardDegreesToPointWRTSectorTip = GetShortestAngularDispDegrees(sectorForwardDegrees, displacementSectorTipToPoint.GetOrientationDegrees());

	if (fabsf(rotationDegreesSectorForwardDegreesToPointWRTSectorTip) < sectorApertureDegrees * 0.5f)
	{
		return GetNearestPointOnDisc2D(referencePosition, sectorTip, sectorRadius);
	}

	if (rotationDegreesSectorForwardDegreesToPointWRTSectorTip > sectorApertureDegrees * 0.5f)
	{
		Vec2 sectorPositiveRotationEndPosition = sectorTip + Vec2::MakeFromPolarDegrees(sectorForwardDegrees + sectorApertureDegrees * 0.5f, sectorRadius);
		return GetNearestPointOnLineSegment2D(referencePosition, sectorTip, sectorPositiveRotationEndPosition);
	}


	Vec2 sectorNegativeRotationEndPosition = sectorTip + Vec2::MakeFromPolarDegrees(sectorForwardDegrees - sectorApertureDegrees * 0.5f, sectorRadius);
	return GetNearestPointOnLineSegment2D(referencePosition, sectorTip, sectorNegativeRotationEndPosition);
}

Vec2 const GetNearestPointOnDirectedSector2D(Vec2 const& referencePosition, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius)
{
	if (IsPointInsideDirectedSector2D(referencePosition, sectorTip, sectorForwardNormal, sectorApertureDegrees, sectorRadius))
	{
		return referencePosition;
	}

	float sectorForwardOrientationDegrees = sectorForwardNormal.GetOrientationDegrees();
	return GetNearestPointOnOrientedSector2D(referencePosition, sectorTip, sectorForwardOrientationDegrees, sectorApertureDegrees, sectorRadius);
}

Vec3 const GetNearestPointOnSphere3D(Vec3 const& referencePosition, Vec3 const& sphereCenter, float sphereRadius)
{
	Vec3 displacementCenterToPoint = referencePosition - sphereCenter;
	return sphereCenter + displacementCenterToPoint.GetClamped(sphereRadius);
}

Vec3 const GetNearestPointOnAABB3(Vec3 const& referencePosition, AABB3 const& box)
{
	return box.GetNearestPoint(referencePosition);
}

Vec3 const GetNearestPointOnOBB3(Vec3 const& referencePosition, OBB3 const& orientedBox)
{
	Vec3 referencePointInOBBLocalSpace = orientedBox.GetLocalPosForWorldPos(referencePosition);
	AABB3 OBB3InLocalSpace(-orientedBox.m_halfDimensions, orientedBox.m_halfDimensions);
	Vec3 nearestPointInOBBLocalSpace = GetNearestPointOnAABB3(referencePointInOBBLocalSpace, OBB3InLocalSpace);
	Vec3 nearestPointInWorldSpace = orientedBox.GetWorldPosForLocalPos(nearestPointInOBBLocalSpace);
	return nearestPointInWorldSpace;
}

Vec3 const GetNearestPointOnCylinder3D(Vec3 const& referencePosition, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius)
{
	// Only works for Z-aligned cylinders right now
	// Transform reference point at the beginning and transform nearest point at the end to implement arbitrary cylinder
	Vec3 nearestPoint;
	nearestPoint = GetNearestPointOnDisc2D(referencePosition.GetXY(), cylinderBaseCenter.GetXY(), cylinderRadius).ToVec3();
	nearestPoint.z = GetClamped(referencePosition.z, cylinderBaseCenter.z, cylinderTopCenter.z);
	return nearestPoint;
}

Vec3 const GetNearestPointOnPlane3(Vec3 const& referencePosition, Plane3 const& plane)
{
	Vec3 displacementPlaneCenterToReferencePosition = referencePosition - plane.GetCenter();
	Vec3 referencePointNormalDispFromPlane = GetProjectedOnto3D(displacementPlaneCenterToReferencePosition, plane.m_normal);
	Vec3 referencePointTangentDistanceFromPlaneCenter = displacementPlaneCenterToReferencePosition - referencePointNormalDispFromPlane;
	return plane.GetCenter() + referencePointTangentDistanceFromPlaneCenter;
}

Vec3 const GetNearestPointOnLineSegment3D(Vec3 const& referencePosition, Vec3 const& lineSegmentStart, Vec3 const& lineSegmentEnd)
{
	Vec3 displacementStartToEnd = lineSegmentEnd - lineSegmentStart;
	Vec3 displacementStartToPoint = referencePosition - lineSegmentStart;

	if (DotProduct3D(displacementStartToEnd, displacementStartToPoint) <= 0)
	{
		return lineSegmentStart;
	}


	Vec3 displacementEndToPoint = referencePosition - lineSegmentEnd;

	if (DotProduct3D(displacementStartToEnd, displacementEndToPoint) >= 0)
	{
		return lineSegmentEnd;
	}

	return lineSegmentStart + GetProjectedOnto3D(displacementStartToPoint, displacementStartToEnd);
}

void TransformPosition2D(Vec2& positionToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation)
{
	positionToTransform *= uniformScale;
	positionToTransform.RotateDegrees(rotationDegrees);
	positionToTransform.x += translation.x;
	positionToTransform.y += translation.y;
}

void TransformPosition2D(Vec2& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	positionToTransform = translation + (iBasis * positionToTransform.x) + (jBasis * positionToTransform.y);
}

void TransformPositionXY3D(Vec3& positionToTransform, float scaleXY, float zRotationDegrees, Vec2 const& translationXY)
{
	positionToTransform.x *= scaleXY;
	positionToTransform.y *= scaleXY;
	positionToTransform = positionToTransform.GetRotatedAboutZDegrees(zRotationDegrees);
	positionToTransform.x += translationXY.x;
	positionToTransform.y += translationXY.y;
}

void TransformPositionXY3D(Vec3& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translationXY)
{
	Vec2 transformedXYPosition = translationXY + (iBasis * positionToTransform.x) + (jBasis * positionToTransform.y);
	positionToTransform = Vec3(transformedXYPosition.x, transformedXYPosition.y, positionToTransform.z);
}

float NormalizeByte(unsigned char byteToNormalize)
{
	return static_cast<float>(byteToNormalize) / 255.f;
}

unsigned char DenormalizeByte(float normalizedValue)
{
	for (int i = 0; i <= 256; i++)
	{
		if ((normalizedValue >= static_cast<float>(i) / 256.f) && (normalizedValue <= static_cast<float>(i + 1) / 256.f))
		{
			return static_cast<unsigned char>(i);
		}
	}

	if (normalizedValue == 0.f)
	{
		return static_cast<unsigned char>(0);
	}

	return static_cast<unsigned char>(255);
}

Mat44 GetBillboardMatrix(BillboardType billboardType, Mat44 const& targetMatrix, Vec3 const& billboardPosition, Vec2 const& billboardScale)
{
	Vec3 targetPosition = targetMatrix.GetTranslation3D();
	Mat44 billboardMatrix = Mat44();

	switch (billboardType)
	{
		case BillboardType::WORLD_UP_FACING:
		{
			Vec3 directionBillboardToTarget = (targetPosition - billboardPosition).GetNormalized();
			Vec3 billboardLeft;
			if (CrossProduct3D(directionBillboardToTarget, Vec3::SKYWARD) == Vec3::ZERO)
			{
				billboardLeft = Vec3::NORTH;
			}
			else
			{
				billboardLeft = CrossProduct3D(Vec3::SKYWARD, directionBillboardToTarget).GetNormalized();
			}
			billboardMatrix.SetIJKT3D(directionBillboardToTarget, billboardLeft, Vec3::SKYWARD, billboardPosition);
			break;
		}
		case BillboardType::WORLD_UP_OPPOSING:
		{
			Vec3 billboardLeft = CrossProduct3D(Vec3::SKYWARD, -targetMatrix.GetIBasis3D());
			billboardMatrix.SetIJKT3D(-targetMatrix.GetIBasis3D(), billboardLeft, Vec3::SKYWARD, billboardPosition);
			break;
		}
		case BillboardType::FULL_FACING:
		{
			Vec3 directionBillboardToTarget = (targetPosition - billboardPosition).GetNormalized();
			Vec3 billboardLeft;
			Vec3 billboardUp;
			if (CrossProduct3D(directionBillboardToTarget, Vec3::SKYWARD) == Vec3::ZERO)
			{
				billboardLeft = Vec3::NORTH;
			}
			else
			{
				billboardLeft = CrossProduct3D(Vec3::SKYWARD, directionBillboardToTarget).GetNormalized();
			}
			billboardUp = CrossProduct3D(directionBillboardToTarget, billboardLeft).GetNormalized();
			billboardMatrix.SetIJKT3D(directionBillboardToTarget, billboardLeft, billboardUp, billboardPosition);
			break;
		}
		case BillboardType::FULL_OPPOSING:
		{
			billboardMatrix.SetIJKT3D(-targetMatrix.GetIBasis3D(), -targetMatrix.GetJBasis3D(), targetMatrix.GetKBasis3D(), billboardPosition);
			break;
		}
	}

	billboardMatrix.AppendScaleNonUniform3D(Vec3(1.f, billboardScale.x, billboardScale.y));
	return billboardMatrix;
}

Vec2 const Interpolate(Vec2 const& startPos, Vec2 const& endPos, float fractionTowardsEnd)
{
	return Vec2(Interpolate(startPos.x, endPos.x, fractionTowardsEnd), Interpolate(startPos.y, endPos.y, fractionTowardsEnd));
}

Vec3 const Interpolate(Vec3 const& startPos, Vec3 const& endPos, float fractionTowardsEnd)
{
	float& t = fractionTowardsEnd;
	return Vec3(Interpolate(startPos.x, endPos.x, t), Interpolate(startPos.y, endPos.y, t), Interpolate(startPos.z, endPos.z, t));
}

float ComputeCubicBezier1D(float A, float B, float C, float D, float t)
{
	float lerpAB = Interpolate(A, B, t);
	float lerpBC = Interpolate(B, C, t);
	float lerpCD = Interpolate(C, D, t);

	float lerpABC = Interpolate(lerpAB, lerpBC, t);
	float lerpBCD = Interpolate(lerpBC, lerpCD, t);

	return Interpolate(lerpABC, lerpBCD, t);
}

float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t)
{
	float lerpAB = Interpolate(A, B, t);
	float lerpBC = Interpolate(B, C, t);
	float lerpCD = Interpolate(C, D, t);
	float lerpDE = Interpolate(D, E, t);
	float lerpEF = Interpolate(E, F, t);

	float lerpABC = Interpolate(lerpAB, lerpBC, t);
	float lerpBCD = Interpolate(lerpBC, lerpCD, t);
	float lerpCDE = Interpolate(lerpCD, lerpDE, t);
	float lerpDEF = Interpolate(lerpDE, lerpEF, t);

	float lerpABCD = Interpolate(lerpABC, lerpBCD, t);
	float lerpBCDE = Interpolate(lerpBCD, lerpCDE, t);
	float lerpCDEF = Interpolate(lerpCDE, lerpDEF, t);

	float lerpABCDE = Interpolate(lerpABCD, lerpBCDE, t);
	float lerpBCDEF = Interpolate(lerpBCDE, lerpCDEF, t);

	return Interpolate(lerpABCDE, lerpBCDEF, t);
}

float SmoothStart2(float t)
{
	return t * t;
}

float EaseInQuadratic(float t)
{
	return SmoothStart2(t);
}

float SmoothStart3(float t)
{
	return t * t * t;
}

float EaseInCubic(float t)
{
	return SmoothStart3(t);
}

float SmoothStart4(float t)
{
	return (t * t) * (t * t);
}

float EaseInQuartic(float t)
{
	return SmoothStart4(t);
}

float SmoothStart5(float t)
{
	return (t * t) * (t * t) * t;
}

float EaseInQuintic(float t)
{
	return SmoothStart5(t);
}

float SmoothStart6(float t)
{
	return (t * t) * (t * t) * (t * t);
}

float EaseInHexic(float t)
{
	return SmoothStart6(t);
}

float SmoothStart7(float t)
{
	return (t * t) * (t * t) * (t * t) * t;
}

float EaseInSeptic(float t)
{
	return SmoothStart7(t);
}

float SmoothStop2(float t)
{
	float tFlipped = 1.f - t;
	float outFlipped = tFlipped * tFlipped;
	return 1.f - outFlipped;
}

float EaseOutQuadratic(float t)
{
	return SmoothStop2(t);
}

float SmoothStop3(float t)
{
	float tFlipped = 1.f - t;
	float outFlipped = tFlipped * tFlipped * tFlipped;
	return 1.f - outFlipped;
}

float EaseOutCubic(float t)
{
	return SmoothStop3(t);
}

float SmoothStop4(float t)
{
	float tFlipped = 1.f - t;
	float outFlipped = (tFlipped * tFlipped) * (tFlipped * tFlipped);
	return 1.f - outFlipped;
}

float EaseOutQuartic(float t)
{
	return SmoothStop4(t);
}

float SmoothStop5(float t)
{
	float tFlipped = 1.f - t;
	float outFlipped = (tFlipped * tFlipped) * (tFlipped * tFlipped) * tFlipped;
	return 1.f - outFlipped;
}

float EaseOutQuintic(float t)
{
	return SmoothStop5(t);
}

float SmoothStop6(float t)
{
	float tFlipped = 1.f - t;
	float outFlipped = (tFlipped * tFlipped) * (tFlipped * tFlipped) * (tFlipped * tFlipped);
	return 1.f - outFlipped;
}

float EaseOutHexic(float t)
{
	return SmoothStop6(t);
}

float SmoothStop7(float t)
{
	float tFlipped = 1.f - t;
	float outFlipped = (tFlipped * tFlipped) * (tFlipped * tFlipped) * (tFlipped * tFlipped) * tFlipped;
	return 1.f - outFlipped;
}

float EaseOutSeptic(float t)
{
	return SmoothStop7(t);
}

float SmoothStep3(float t)
{
	return Interpolate(EaseInQuadratic(t), EaseOutQuadratic(t), t);
}

float SmoothStep(float t)
{
	return SmoothStep3(t);
}

float SmoothStep5(float t)
{
	return Interpolate(EaseInQuartic(t), EaseOutQuartic(t), t);
}

float SmootherStep(float t)
{
	return SmoothStep5(t);
}

float Hesitate3(float t)
{
	return ComputeCubicBezier1D(0.f, 1.f, 0.f, 1.f, t);
}

float Hesitate5(float t)
{
	return ComputeQuinticBezier1D(0.f, 1.f, 0.f, 1.f, 0.f, 1.f, t);
}

bool AreVelocitiesConverging2D(Vec2 const& velocityA, Vec2 const& velocityB, Vec2 const& normalAToB)
{
	float normalVelocityA = GetProjectedLength2D(velocityA, normalAToB);
	float normalVelocityB = GetProjectedLength2D(velocityB, normalAToB);

	float relativeVelocityAlongNormalBWrtA = normalVelocityB - normalVelocityA;
	return (relativeVelocityAlongNormalBWrtA < 0.f);
}

bool AreVelocitiesDiverging2D(Vec2 const& velocityA, Vec2 const& velocityB, Vec2 const& normalAToB)
{
	return !AreVelocitiesConverging2D(velocityA, velocityB, normalAToB);
}

EulerAngles const GetEulerAnglesFromQuaternion(float quaternionX, float quaternionY, float quaternionZ, float quaternionW)
{
	float quaternionLength = Vec4(quaternionX, quaternionY, quaternionZ, quaternionW).GetLength();

	if (quaternionLength == 0.f)
	{
		return EulerAngles::ZERO;
	}

	float normalizedQX = quaternionX / quaternionLength;
	float normalizedQY = quaternionY / quaternionLength;
	float normalizedQZ = quaternionZ / quaternionLength;
	float normalizedQW = quaternionW / quaternionLength;

	// Roll
	float sinR_cosP = 2.f * (normalizedQW * normalizedQX + normalizedQY * normalizedQZ);
	float cosR_cosP = 1.f - 2.f * (normalizedQX * normalizedQX + normalizedQY * normalizedQY);
	float roll = atan2f(sinR_cosP, cosR_cosP);

	// Pitch
	float sinP = 2.f * (normalizedQW * normalizedQY - normalizedQZ * normalizedQX);
	float pitch;
	if (fabsf(sinP) >= 1.f)
	{
		pitch = copysignf(PI / 2.f, sinP);
	}
	else
	{
		pitch = asinf(sinP);
	}

	// Yaw
	float sinY_cosP = 2.f * (normalizedQW * normalizedQZ + normalizedQX * normalizedQY);
	float cosY_cosP = 1.f - 2.f * (normalizedQY * normalizedQY + normalizedQZ * normalizedQZ);
	float yaw = atan2f(sinY_cosP, cosY_cosP);

	return EulerAngles(ConvertRadiansToDegrees(yaw), ConvertRadiansToDegrees(pitch), ConvertRadiansToDegrees(roll));
}
