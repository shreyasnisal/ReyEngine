#pragma once

#include "Engine/Math/Vec2.hpp"

#include <vector>


struct Vec3;
struct Vec4;
struct AABB2;
struct AABB3;
struct OBB2;
struct OBB3;
struct IntVec2;
struct Rgba8;
struct Mat44;
struct Plane2;
struct Plane3;
class EulerAngles;

constexpr float PI = 3.1415926535897932384626433832795f;

enum class BillboardType
{
	NONE = -1,
	WORLD_UP_FACING,
	WORLD_UP_OPPOSING,
	FULL_FACING,
	FULL_OPPOSING,
	COUNT
};

/*! \file
* \brief This file contains math helper functions
* 
*/

bool				AreFloatsMostlyEqual(float val1, float val2, float tolerance);
float				GetClamped(float value, float minValue, float maxValue);
float				GetClampedZeroToOne(float value);
float				Interpolate(float start, float end, float fractionTowardEnd);
float				GetFractionWithinRange(float value, float rangeStart, float rangeEnd);
float				RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float				RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int					RoundDownToInt(float value);
float				GetMin(float a, float b);
float				GetMin(std::vector<float> const& arr);
float				GetMax(float a, float b);
int					GetMax(int a, int b);
int					GetMax(int numElements, int* arr);

float				CosDegrees(float thetaDegrees);
float				SinDegrees(float thetaDegrees);
float				TanDegrees(float thetaDegrees);
float				AcosDegrees(float value);
float				AsinDegrees(float value);
float				Atan2Degrees(float y, float x);
float				ConvertRadiansToDegrees(float radians);
float				ConvertDegreesToRadians(float radians);
float				GetShortestAngularDispDegrees(float startDegrees, float endDegrees);
float				GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees);
float				GetAngleDegreesBetweenVectors2D(Vec2 const& vecA, Vec2 const& vecB);
float				GetAngleDegreesBetweenVectors3D(Vec3 const& vecA, Vec3 const& vecB);


float				DotProduct2D(Vec2 const& a, Vec2 const& b);
float				DotProduct3D(Vec3 const& a, Vec3 const& b);
float				DotProduct4D(Vec4 const& a, Vec4 const& b);

float				CrossProduct2D(Vec2 const& a, Vec2 const& b);
Vec3				CrossProduct3D(Vec3 const& a, Vec3 const& b);

float				GetDistance2D(Vec2 const& positionA, Vec2 const& positionB);
float				GetDistanceSquared2D(Vec2 const& positionA, Vec2 const& positionB);
float				GetDistance3D(Vec3 const& positionA, Vec3 const& positionB);
float				GetDistanceSquared3D(Vec3 const& positionA, Vec3 const& positionB);
float				GetDistanceXY3D(Vec3 const& positionA, Vec3 const& positionB);
float				GetDistanceXYSquared3D(Vec3 const& positionA, Vec3 const& positionB);
int					GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB);
float				GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);
Vec2 const			GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto);
float				GetProjectedLength3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto);
Vec3 const			GetProjectedOnto3D(Vec3 const& vectorToProject, Vec3 const& vectorToProjectOnto);

bool				IsPointInsideAABB2(Vec2 const& point, AABB2 const& box);
bool				IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
bool				IsPointInsideOBB2(Vec2 const& point, OBB2 const& orientedBox);
bool				IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius);
bool				IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius);
bool				IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius);
bool				IsPointInsideSphere3D(Vec3 const& point, Vec3 const& sphereCenter, float sphereRadius);
bool				IsPointInsideAABB3(Vec3 const& point, AABB3 const& box);
bool				IsPointInsideOBB3(Vec3 const& point, OBB3 const& orientedBox);
bool				IsPointInsideCylinder3D(Vec3 const& point, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius);
bool				IsPointInsideCapsule3D(Vec3 const& point, Vec3 const& capsuleStart, Vec3 const& capsuleEnd, float capsuleRadius);

bool				DoDiscsOverlap(Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB);
bool				DoAABB2Overlap(AABB2 const& boxA, AABB2 const& boxB);
bool				DoDiscAndAABB2Overlap(Vec2 const& discCenter, float discRadius, AABB2 const& box);
bool				DoAABB2AndDiscOverlap(AABB2 const& box, Vec2 const& discCenter, float discRadius);
bool				DoDiscAndCapsuleOverlap(Vec2 const& discCenter, float discRadius, Vec2 const& capsuleStart, Vec2 const& capsuleEnd, float capsuleRadius);
bool				DoCapsuleAndDiscOverlap(Vec2 const& capsuleStart, Vec2 const& capsuleEnd, float capsuleRadius, Vec2 const& discCenter, float discRadius);
bool				DoDiscAndOBB2Overlap(Vec2 const& discCenter, float discRadius, OBB2 const& box);
bool				DoOBB2AndDiscOverlap(OBB2 const& box, Vec2 const& discCenter, float discRadius);
bool				DoSpheresOverlap(Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool				DoAABB3Overlap(AABB3 const& boxA, AABB3 const& boxB);
bool				DoSphereAndAABB3Overlap(Vec3 const& sphereCenter, float radius, AABB3 const& box);
bool				DoAABB3AndSphereOverlap(AABB3 const& box, Vec3 const& sphereCenter, float radius);
bool				DoSphereAndCylinderOverlap(Vec3 const& sphereCenter, float sphereRadius, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius);
bool				DoCylinderAndSphereOverlap(Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius, Vec3 const& sphereCenter, float sphereRadius);
bool				DoAABB3AndZCylinderOverlap(AABB3 const& box, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius);
bool				DoZCylinderAndAABB3Overlap(Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius, AABB3 const& box);
bool				DoZCylindersOverlap(Vec3 const& cylinderABaseCenter, Vec3 const& cylinderATopCenter, float cylinderARadius, Vec3 const& cylinderBBaseCenter, Vec3 const& cylinderBTopCenter, float cylinderBRadius);
bool				DoOBB3AndPlane3Overlap(OBB3 const& orientedBox, Plane3 const& plane);
bool				DoPlane3AndOBB3Overlap(Plane3 const& plane, OBB3 const& orientedBox);
bool				DoSphereAndPlane3Overlap(Vec3 const& sphereCenter, float sphereRadius, Plane3 const& plane);
bool				DoPlane3AndSphereOverlap(Plane3 const& plane, Vec3 const& sphereCenter, float sphereRadius);
bool				DoOBB3AndSphereOverlap(OBB3 const& orientedBox, Vec3 const& sphereCenter, float sphereRadius);
bool				DoSphereAndOBB3Overlap(Vec3 const& sphereCenter, float sphereRadius, OBB3 const& orientedBox);
bool				DoPlane3AndAABB3Overlap(Plane3 const& plane, AABB3 const& box);
bool				DoAABB3AndPlane3Overlap(AABB3 const& box, Plane3 const& plane);

bool				PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedPoint);
bool				PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius);
bool				PushDiscIntoFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius);
bool				PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius);
bool				PushDiscOutOfFixedAABB2(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox);
bool				PushDiscOutOfFixedOBB2(Vec2& mobileDiscCenter, float mobileDiscRadius, OBB2 const& fixedOrientedBox);
bool				PushDiscOutOfFixedCapsule2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedCapsuleStart, Vec2 const& fixedCapsuleEnd, float capsuleRadius);
bool				PushSphereOutOfFixedPoint3D(Vec3& mobileSphereCenter, float mobileSphereRadius, Vec3 const& fixedPoint);
bool				PushSphereOutOfFixedSphere3D(Vec3 mobileSphereCenter, float mobileSphereRadius, Vec3 const& fixedSphereCenter, float fixedSphereRadius);
bool				PushSpheresOutOfEachOther3D(Vec3& aCenter, float aRadius, Vec3& bCenter, float bRadius);
bool				PushZCylinderOutOfFixedAABB3(Vec3& cylinderBaseCenter, Vec3& cylinderTopCenter, float cylinderRadius, AABB3 const& box);
bool				PushAABB3OutOfFixedZCylinder(AABB3& box, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius);
bool				PushSphereOutOfFixedAABB3(Vec3& sphereCenter, float sphereRadius, AABB3 const& fixedBox);
bool				PushSphereOutOfFixedOBB3(Vec3& sphereCenter, float sphereRadius, OBB3 const& fixedOrientedBox);

bool				BounceDiscsOffEachOther2D(Vec2& aCenter, float aRadius, Vec2& aVelocity, float aElasticity, Vec2& bCenter, float bRadius, Vec2& bVelocity, float bElasticity);
bool				BounceDiscOffFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, Vec2 const& fixedDiscCenter, float fixedDiscRadius, float fixedDiscElasticity);
bool				BounceDiscOffFixedOBB2(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, OBB2 const& fixedBox, float boxElasticity);
bool				BounceDiscOffFixedAABB2(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, AABB2 const& fixedBox, float boxElasticity);
bool				BounceDiscOffFixedCapsule2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2& mobileDiscVelocity, float mobileDiscElasticity, Vec2 const& fixedCapsuleStart, Vec2 const& fixedCapsuleEnd, float fixedCapsuleRadius, float capsuleElasticity);

Vec2 const			GetNearestPointOnDisc2D(Vec2 const& referencePosition, Vec2 const& discCenter, float discRadius);
Vec2 const			GetNearestPointOnAABB2(Vec2 const& referencePosition, AABB2 const& box);
Vec2 const			GetNearestPointOnInfiniteLine2D(Vec2 const& referencePosition, Vec2 const& linePointA, Vec2 const& linePointB);
Vec2 const			GetNearestPointOnLineSegment2D(Vec2 const& referencePosition, Vec2 const& lineSegmentStart, Vec2 const& lineSegmentEnd);
Vec2 const			GetNearestPointOnCapsule2D(Vec2 const& referencePosition, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
Vec2 const			GetNearestPointOnOBB2(Vec2 const& referencePosition, OBB2 const& orientedBox);
Vec2 const			GetNearestPointOnOrientedSector2D(Vec2 const& referencePosition, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius);
Vec2 const			GetNearestPointOnDirectedSector2D(Vec2 const& referencePosition, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius);
Vec3 const			GetNearestPointOnSphere3D(Vec3 const& referencePosition, Vec3 const& sphereCenter, float sphereRadius);
Vec3 const			GetNearestPointOnAABB3(Vec3 const& referencePosition, AABB3 const& box);
Vec3 const			GetNearestPointOnOBB3(Vec3 const& referencePosition, OBB3 const& orientedBox);
Vec3 const			GetNearestPointOnCylinder3D(Vec3 const& referencePosition, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius);
Vec3 const			GetNearestPointOnPlane3(Vec3 const& referencePosition, Plane3 const& plane);
Vec3 const			GetNearestPointOnLineSegment3D(Vec3 const& referencePosition, Vec3 const& lineSegmentStart, Vec3 const& lineSegmentEnd);

void				TransformPosition2D(Vec2& positionToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation);
void				TransformPosition2D(Vec2& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void				TransformPositionXY3D(Vec3& positionToTransform, float scaleXY, float xRotationDegrees, Vec2 const& translationXY);
void				TransformPositionXY3D(Vec3& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translationXY);

float				NormalizeByte(unsigned char byteToNormalize);
unsigned char		DenormalizeByte(float normalizedValue);

Rgba8 const			Interpolate(Rgba8 const& startColor, Rgba8 const& endColor, float fractionTowardsEnd);
Vec2 const			Interpolate(Vec2 const& startPos, Vec2 const& endPos, float fractionTowardsEnd);
Vec3 const			Interpolate(Vec3 const& startPos, Vec3 const& endPos, float fractionTowardsEnd);

Mat44				GetBillboardMatrix(BillboardType billboardType, Mat44 const& cameraMatrix, Vec3 const& billboardPosition, Vec2 const& billboardScale = Vec2::ONE);

float				ComputeCubicBezier1D(float A, float B, float C, float D, float t);
float				ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t);

float				SmoothStart2(float t);
float				EaseInQuadratic(float t);
float				SmoothStart3(float t);
float				EaseInCubic(float t);
float				SmoothStart4(float t);
float				EaseInQuartic(float t);
float				SmoothStart5(float t);
float				EaseInQuintic(float t);
float				SmoothStart6(float t);
float				EaseInHexic(float t);
float				SmoothStart7(float t);
float				EaseInSeptic(float t);

float				SmoothStop2(float t);
float				EaseOutQuadratic(float t);
float				SmoothStop3(float t);
float				EaseOutCubic(float t);
float				SmoothStop4(float t);
float				EaseOutQuartic(float t);
float				SmoothStop5(float t);
float				EaseOutQuintic(float t);
float				SmoothStop6(float t);
float				EaseOutHexic(float t);
float				SmoothStop7(float t);
float				EaseOutSeptic(float t);

float				SmoothStep3(float t);
float				SmoothStep(float t);
float				SmoothStep5(float t);
float				SmootherStep(float t);

float				Hesitate3(float t);
float				Hesitate5(float t);

bool				AreVelocitiesConverging2D(Vec2 const& velocityA, Vec2 const& velocityB, Vec2 const& normalAToB);
bool				AreVelocitiesDiverging2D(Vec2 const& velocityA, Vec2 const& velocityB, Vec2 const& normalAToB);

EulerAngles const	GetEulerAnglesFromQuaternion(float quaternionX, float quaternionY, float quaternionZ, float quaternionW);
