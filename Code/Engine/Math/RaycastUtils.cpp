#include "RaycastUtils.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"

#include <math.h>
#include <float.h>

RaycastResult2D RaycastVsDisc2D(Vec2 const& startPos, Vec2 const& fwdNormal, float maxDistance, Vec2 const& discCenter, float discRadius)
{
	Vec2 rayLeftNormal = fwdNormal.GetRotated90Degrees();

	Vec2 displacementStartPosToDiscCenter = discCenter - startPos;
	float discDistanceAlongRay = GetProjectedLength2D(displacementStartPosToDiscCenter, fwdNormal);
	float discAltitude = GetProjectedLength2D(displacementStartPosToDiscCenter, rayLeftNormal);

	RaycastResult2D raycastResult;

	if (discAltitude >= discRadius)
	{
		// miss left
		return raycastResult;
	}
	if (discAltitude <= -discRadius)
	{
		// miss right
		return raycastResult;
	}

	if (IsPointInsideDisc2D(startPos, discCenter, discRadius))
	{
		// immediate hit
		raycastResult.m_didImpact = true;
		raycastResult.m_impactPosition = startPos;
		raycastResult.m_impactNormal = -fwdNormal;
	}

	float impactDistanceFromDiscDistance = sqrtf((discRadius * discRadius) - (discAltitude * discAltitude));
	float impactDistance = discDistanceAlongRay - impactDistanceFromDiscDistance;
	
	if (impactDistance <= 0.f)
	{
		// miss early
		return raycastResult;
	}
	if (impactDistance >= maxDistance)
	{
		// miss late
		return raycastResult;
	}

	raycastResult.m_impactDistance = impactDistance;
	raycastResult.m_didImpact = true;
	raycastResult.m_impactPosition = startPos + impactDistance * fwdNormal;
	raycastResult.m_impactNormal = (raycastResult.m_impactPosition - discCenter).GetNormalized();

	return raycastResult;
}

RaycastResult2D RaycastVsLineSegment2D(Vec2 const& startPos, Vec2 const& fwdNormal, float maxDistance, Vec2 const& lineSegmentStart, Vec2 const& lineSegmentEnd)
{
	RaycastResult2D raycastResult;
	raycastResult.m_rayStartPosition = startPos;
	raycastResult.m_rayForwardNormal = fwdNormal;
	raycastResult.m_rayMaxLength = maxDistance;

	Vec2 rayJBasis = fwdNormal.GetRotated90Degrees();

	Vec2 dispRayStartToLineSegmentStart = lineSegmentStart - startPos;
	Vec2 dispRayStartToLineSegmentEnd = lineSegmentEnd - startPos;
	float projDispStartToStartJ = GetProjectedLength2D(dispRayStartToLineSegmentStart, rayJBasis);
	float projDispStartToEndJ = GetProjectedLength2D(dispRayStartToLineSegmentEnd, rayJBasis);

	if (projDispStartToStartJ * projDispStartToEndJ >= 0.f)
	{
		// miss same side
		return raycastResult;
	}

	float fractionOnLineSegment = projDispStartToStartJ / (projDispStartToStartJ - projDispStartToEndJ);
	Vec2 dispLineSegmentStartToEnd = lineSegmentEnd - lineSegmentStart;
	Vec2 impactPosition = lineSegmentStart + fractionOnLineSegment * dispLineSegmentStartToEnd;
	Vec2 dispRayStartToImpactPosition = impactPosition - startPos;
	float impactDistance = DotProduct2D(dispRayStartToImpactPosition, fwdNormal);

	if (impactDistance >= maxDistance)
	{
		// miss late
		return raycastResult;
	}
	if (impactDistance <= 0.f)
	{
		// miss early
		return raycastResult;
	}

	raycastResult.m_didImpact = true;
	raycastResult.m_impactDistance = impactDistance;
	raycastResult.m_impactPosition = impactPosition;
	raycastResult.m_impactNormal = (projDispStartToEndJ * dispLineSegmentStartToEnd.GetRotated90Degrees()).GetNormalized();

	return raycastResult;
}

RaycastResult2D RaycastVsAABB2(Vec2 const& startPos, Vec2 const& fwdNormal, float maxDistance, AABB2 const& box)
{
	RaycastResult2D raycastResult;
	raycastResult.m_rayStartPosition = startPos;
	raycastResult.m_rayForwardNormal = fwdNormal;
	raycastResult.m_rayMaxLength = maxDistance;

	if (IsPointInsideAABB2(startPos, box))
	{
		// report immedate hit, impactNormal opposing ray direction
		raycastResult.m_didImpact = true;
		raycastResult.m_impactDistance = 0.f;
		raycastResult.m_impactPosition = startPos;
		raycastResult.m_impactNormal = -fwdNormal;
	}
	
	Vec2 line1Start;
	Vec2 line1End;
	Vec2 line2Start;
	Vec2 line2End;
	
	if (fwdNormal.x < 0.f)
	{
		// can only hit the right side of the box
		line1Start = Vec2(box.m_maxs.x, box.m_mins.y);
		line1End = box.m_maxs;
	}
	else
	{
		// can only hit the left side of the box
		line1Start = box.m_mins;
		line1End = Vec2(box.m_mins.x, box.m_maxs.y);
	}

	if (fwdNormal.y < 0.f)
	{
		// can only hit the top of the box
		line2Start = Vec2(box.m_mins.x, box.m_maxs.y);
		line2End = box.m_maxs;
	}
	else
	{
		// can only hit the bottom of the box
		line2Start = box.m_mins;
		line2End = Vec2(box.m_maxs.x, box.m_mins.y);
	}

	// perform 2 raycasts with those lines, report closest
	RaycastResult2D raycastVsLine1 = RaycastVsLineSegment2D(startPos, fwdNormal, maxDistance, line1Start, line1End);
	RaycastResult2D raycastVsLine2 = RaycastVsLineSegment2D(startPos, fwdNormal, maxDistance, line2Start, line2End);

	if (!raycastVsLine1.m_didImpact && !raycastVsLine2.m_didImpact)
	{
		// report miss
		return raycastResult;
	}
	if (!raycastVsLine2.m_didImpact)
	{
		return raycastVsLine1;
	}
	if (!raycastVsLine1.m_didImpact)
	{
		return raycastVsLine2;
	}

	if (raycastVsLine1.m_impactDistance < raycastVsLine2.m_impactDistance)
	{
		return raycastVsLine1;
	}
	else
	{
		return raycastVsLine2;
	}
}

RaycastResult3D RaycastVsCylinder3D(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, Vec3 const& cylinderBaseCenter, Vec3 const& cylinderTopCenter, float cylinderRadius)
{
	RaycastResult3D result;
	result.m_rayStartPosition = startPos;
	result.m_rayForwardNormal = fwdNormal;
	result.m_rayMaxLength = maxDistance;

	Vec3 cylinderUp = cylinderTopCenter - cylinderBaseCenter;
	float cylinderHeight = cylinderUp.GetLength();
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

	Vec3 rayLocalStartPos = worldToCylinderLocalMatrix.TransformPosition3D(startPos);
	Vec3 rayLocalFwdNormal = worldToCylinderLocalMatrix.TransformVectorQuantity3D(fwdNormal);
	rayLocalFwdNormal = rayLocalFwdNormal.GetNormalized();

	Vec2 rayLocalStartPos2D = rayLocalStartPos.GetXY();
	Vec2 rayLocalFwdNormal2D = rayLocalFwdNormal.GetXY().GetNormalized();
	RaycastResult2D crossSectionRaycastResult = RaycastVsDisc2D(rayLocalStartPos2D, rayLocalFwdNormal2D, maxDistance, Vec2::ZERO, cylinderRadius);
	bool didImpactCrossSection = false;
	if (crossSectionRaycastResult.m_didImpact)
	{
		float crossSectionRaycastDistance = (crossSectionRaycastResult.m_impactPosition.x - rayLocalStartPos.x) / rayLocalFwdNormal.x;
		if (crossSectionRaycastDistance < 0.f)
		{
			crossSectionRaycastDistance = FLT_MAX;
		}
		float crossSectionRaycastLocalZ = rayLocalStartPos.z + rayLocalFwdNormal.z * crossSectionRaycastDistance;
		if (crossSectionRaycastLocalZ > 0.f && crossSectionRaycastLocalZ < cylinderHeight)
		{
			didImpactCrossSection = true;
			result.m_didImpact = true;
			result.m_impactDistance = crossSectionRaycastDistance;
			Vec3 impactLocalNormal = Vec3(crossSectionRaycastResult.m_impactNormal.x, crossSectionRaycastResult.m_impactNormal.y, 0.f);
			result.m_impactNormal = cylinderLocalToWorldMatrix.TransformVectorQuantity3D(impactLocalNormal);
		}
	}

	float rayBasePlaneImpactDistance = -rayLocalStartPos.z / rayLocalFwdNormal.z;
	if (rayBasePlaneImpactDistance < 0.f)
	{
		rayBasePlaneImpactDistance = FLT_MAX;
	}
	Vec3 rayBasePlaneImpactPoint = rayLocalStartPos + rayLocalFwdNormal * rayBasePlaneImpactDistance;
	bool didImpactAtBase = false;
	if (IsPointInsideDisc2D(rayBasePlaneImpactPoint.GetXY(), Vec2::ZERO, cylinderRadius))
	{
		didImpactAtBase = true;
		if (result.m_didImpact)
		{
			if (rayBasePlaneImpactDistance < result.m_impactDistance)
			{
				result.m_impactDistance = rayBasePlaneImpactDistance;
				result.m_impactNormal = cylinderLocalToWorldMatrix.TransformVectorQuantity3D(Vec3::GROUNDWARD);
			}
		}
		else
		{
			result.m_impactDistance = rayBasePlaneImpactDistance;
			result.m_impactNormal = cylinderLocalToWorldMatrix.TransformVectorQuantity3D(Vec3::GROUNDWARD);
		}
		result.m_didImpact = true;
	}

	float rayTopPlaneImpactDistance = (cylinderHeight - rayLocalStartPos.z) / rayLocalFwdNormal.z;
	if (rayTopPlaneImpactDistance < 0.f)
	{
		rayTopPlaneImpactDistance = FLT_MAX;
	}
	Vec3 rayTopPlaneImpactPoint = rayLocalStartPos + rayLocalFwdNormal * rayTopPlaneImpactDistance;
	bool didImpactAtTop = false;
	if (IsPointInsideDisc2D(rayTopPlaneImpactPoint.GetXY(), Vec2::ZERO, cylinderRadius))
	{
		didImpactAtTop = true;
		if ( result.m_didImpact)
		{
			if (rayTopPlaneImpactDistance < result.m_impactDistance)
			{
				result.m_impactDistance = rayTopPlaneImpactDistance;
				result.m_impactNormal = cylinderLocalToWorldMatrix.TransformVectorQuantity3D(Vec3::SKYWARD);
			}
		}
		else
		{
			result.m_impactDistance = rayTopPlaneImpactDistance;
			result.m_impactNormal = cylinderLocalToWorldMatrix.TransformVectorQuantity3D(Vec3::SKYWARD);
		}
		result.m_didImpact = true;
	}

	result.m_impactPosition = startPos + fwdNormal * result.m_impactDistance;

	if (result.m_impactDistance > maxDistance)
	{
		result.m_didImpact = false;
		result.m_impactDistance = 0.f;
		result.m_impactPosition = Vec3::ZERO;
		result.m_impactNormal = Vec3::ZERO;
	}

	return result;
}

RaycastResult3D RaycastVsSphere(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, Vec3 const& sphereCenter, float sphereRadius)
{
	RaycastResult3D raycastResult;
	raycastResult.m_rayStartPosition = startPos;
	raycastResult.m_rayForwardNormal = fwdNormal;
	raycastResult.m_rayMaxLength = maxDistance;

	if (IsPointInsideSphere3D(startPos, sphereCenter, sphereRadius))
	{
		raycastResult.m_didImpact = true;
		raycastResult.m_impactDistance = 0.f;
		raycastResult.m_impactPosition = startPos;
		raycastResult.m_impactNormal = -fwdNormal;
		return raycastResult;
	}

	Vec3 dispStartToCenter = sphereCenter - startPos;
	float distanceAlongRay = DotProduct3D(dispStartToCenter, fwdNormal);

	if (distanceAlongRay <= 0.f)
	{
		return raycastResult;
	}
	if (distanceAlongRay >= maxDistance + sphereRadius)
	{
		return raycastResult;
	}

	Vec3 dispAlongRay = distanceAlongRay * fwdNormal;
	Vec3 dispAlongPerpendicular = dispStartToCenter - dispAlongRay;
	float distAlongPerpendicularSq = dispAlongPerpendicular.GetLengthSquared();

	if (distAlongPerpendicularSq > sphereRadius * sphereRadius)
	{
		return raycastResult;
	}

	float backUpDistance = sqrtf(sphereRadius * sphereRadius - distAlongPerpendicularSq);
	float impactDistance = distanceAlongRay - backUpDistance;
	if (impactDistance <= 0.f || impactDistance >= maxDistance)
	{
		return raycastResult;
	}

	raycastResult.m_didImpact = true;
	raycastResult.m_impactDistance = impactDistance;
	raycastResult.m_impactPosition = startPos + impactDistance * fwdNormal;
	raycastResult.m_impactNormal = (raycastResult.m_impactPosition - sphereCenter).GetNormalized();

	return raycastResult;
}

RaycastResult3D RaycastVsAABB3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, AABB3 const& box)
{
	RaycastResult3D raycastResult;
	raycastResult.m_rayStartPosition = startPos;
	raycastResult.m_rayForwardNormal = fwdNormal;
	raycastResult.m_rayMaxLength = maxDistance;

	if (IsPointInsideAABB3(startPos, box))
	{
		// report immediate hit, impactNormal opposing ray direction
		raycastResult.m_didImpact = true;
		raycastResult.m_impactDistance = 0.f;
		raycastResult.m_impactPosition = startPos;
		raycastResult.m_impactNormal = -fwdNormal;
	}

	float xPlane = box.m_mins.x;
	float yPlane = box.m_mins.y;
	float zPlane = box.m_mins.z;

	if (fwdNormal.x < 0.f)
	{
		// can only hit the far face of the box
		xPlane = box.m_maxs.x;
	}

	if (fwdNormal.y < 0.f)
	{
		// can only hit the right face of the box
		yPlane = box.m_maxs.y;
	}

	if (fwdNormal.z < 0.f)
	{
		// can only hit the top face of the box
		zPlane = box.m_maxs.z;
	}

	// perform 3 raycasts with correct faces, report closest
	float xPlaneImpactDistance = (xPlane - startPos.x) / fwdNormal.x;
	if (xPlaneImpactDistance < 0.f || xPlaneImpactDistance > maxDistance)
	{
		xPlaneImpactDistance = maxDistance;
	}
	else
	{
		Vec3 xPlaneImpactPosition = startPos + fwdNormal * xPlaneImpactDistance;
		if (IsPointInsideAABB2(xPlaneImpactPosition.GetYZ(), AABB2(Vec2(box.m_mins.y, box.m_mins.z), Vec2(box.m_maxs.y, box.m_maxs.z))))
		{
			raycastResult.m_didImpact = true;
			raycastResult.m_impactDistance = xPlaneImpactDistance;
			raycastResult.m_impactPosition = xPlaneImpactPosition;
			raycastResult.m_impactNormal = fwdNormal.x > 0.f ? Vec3(-1.f, 0.f, 0.f) : Vec3(1.f, 0.f, 0.f);
		}
		else
		{
			xPlaneImpactDistance = maxDistance;
		}
	}

	float yPlaneImpactDistance = (yPlane - startPos.y) / fwdNormal.y;
	if (yPlaneImpactDistance < 0.f || yPlaneImpactDistance > maxDistance)
	{
		yPlaneImpactDistance = maxDistance;
	}
	else
	{
		Vec3 yPlaneImpactPosition = startPos + fwdNormal * yPlaneImpactDistance;
		if (IsPointInsideAABB2(yPlaneImpactPosition.GetZX(), AABB2(Vec2(box.m_mins.z, box.m_mins.x), Vec2(box.m_maxs.z, box.m_maxs.x))))
		{
			if (yPlaneImpactDistance < xPlaneImpactDistance)
			{
				raycastResult.m_didImpact = true;
				raycastResult.m_impactDistance = yPlaneImpactDistance;
				raycastResult.m_impactPosition = yPlaneImpactPosition;
				raycastResult.m_impactNormal = fwdNormal.y > 0.f ? Vec3(0.f, -1.f, 0.f) : Vec3(0.f, 1.f, 0.f);
			}
		}
		else
		{
			yPlaneImpactDistance = maxDistance;
		}
	}

	float zPlaneImpactDistance = (zPlane - startPos.z) / fwdNormal.z;
	if (zPlaneImpactDistance < 0.f || zPlaneImpactDistance > maxDistance)
	{
		zPlaneImpactDistance = maxDistance;
	}
	else
	{
		Vec3 zPlaneImpactPosition = startPos + fwdNormal * zPlaneImpactDistance;
		if (IsPointInsideAABB2(zPlaneImpactPosition.GetXY(), AABB2(Vec2(box.m_mins.x, box.m_mins.y), Vec2(box.m_maxs.x, box.m_maxs.y))))
		{
			if (zPlaneImpactDistance < yPlaneImpactDistance && zPlaneImpactDistance < xPlaneImpactDistance)
			{
				raycastResult.m_didImpact = true;
				raycastResult.m_impactDistance = zPlaneImpactDistance;
				raycastResult.m_impactPosition = zPlaneImpactPosition;
				raycastResult.m_impactNormal = fwdNormal.z > 0.f ? Vec3(0.f, 0.f, -1.f) : Vec3(0.f, 0.f, 1.f);
			}
		}
		else
		{
			zPlaneImpactDistance = maxDistance;
		}
	}

	return raycastResult;
}

RaycastResult3D RaycastVsOBB3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, OBB3 const& orientedBox)
{
	Mat44 obbTransformMatrix = Mat44(orientedBox.m_iBasis, orientedBox.m_jBasis, orientedBox.m_kBasis, orientedBox.m_center);
	Mat44 obbTransformMatrixInverse = obbTransformMatrix.GetOrthonormalInverse();

	RaycastResult3D raycastResult;
	raycastResult.m_rayStartPosition = startPos;
	raycastResult.m_rayForwardNormal = fwdNormal;
	raycastResult.m_rayMaxLength = maxDistance;

	Vec3 rayStartInObbLocalSpace = obbTransformMatrixInverse.TransformPosition3D(startPos);
	Vec3 fwdNormalInObbLocalSpace = obbTransformMatrixInverse.TransformVectorQuantity3D(fwdNormal);
	RaycastResult3D raycastVsAABBResult = RaycastVsAABB3(rayStartInObbLocalSpace, fwdNormalInObbLocalSpace, maxDistance, AABB3(-orientedBox.m_halfDimensions, orientedBox.m_halfDimensions));

	raycastResult.m_didImpact = raycastVsAABBResult.m_didImpact;
	raycastResult.m_impactDistance = raycastVsAABBResult.m_impactDistance;
	raycastResult.m_impactPosition = obbTransformMatrix.TransformPosition3D(raycastVsAABBResult.m_impactPosition);
	raycastResult.m_impactNormal = obbTransformMatrix.TransformVectorQuantity3D(raycastVsAABBResult.m_impactNormal);

	return raycastResult;
}

RaycastResult3D RaycastVsPlane3(Vec3 const& startPos, Vec3 const& fwdNormal, float maxDistance, Plane3 const& plane)
{
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

	Mat44 planeTransform = Mat44(planeIBasis, planeJBasis, plane.m_normal, planeCenter);
	Mat44 planeTransformInverse = planeTransform.GetOrthonormalInverse();

	RaycastResult3D raycastResult;
	raycastResult.m_rayStartPosition = startPos;
	raycastResult.m_rayForwardNormal = fwdNormal;
	raycastResult.m_rayMaxLength = maxDistance;

	Vec3 rayStartInPlaneLocalSpace = planeTransformInverse.TransformPosition3D(startPos);
	Vec3 fwdNormalInPlaneLocalSpace = planeTransformInverse.TransformVectorQuantity3D(fwdNormal);

	float rayPlaneImpactDistance = -rayStartInPlaneLocalSpace.z / fwdNormalInPlaneLocalSpace.z;
	if (rayPlaneImpactDistance < 0.f || rayPlaneImpactDistance > maxDistance)
	{
		return raycastResult;
	}
	else
	{
		raycastResult.m_didImpact = true;
		raycastResult.m_impactDistance = rayPlaneImpactDistance;
		raycastResult.m_impactPosition = startPos + rayPlaneImpactDistance * fwdNormal;
		raycastResult.m_impactNormal = plane.IsPointBehind(startPos) ? -plane.m_normal : plane.m_normal;
	}

	return raycastResult;
}
