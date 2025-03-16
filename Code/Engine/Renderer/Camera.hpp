#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"

class Camera
{
public:
	enum Mode
	{
		MODE_ORTHOGRAPHIC,
		MODE_PERSPECTIVE,
		MODE_XR,

		MODE_COUNT
	};

public:	
	~Camera() = default;
	Camera() = default;

	void SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight, float near = 0.f, float far = 1.f);
	void SetPerspectiveView(float aspect, float fov, float near, float far);
	void SetXRView(float angleLeft, float angleRight, float angleUp, float angleDown, float near, float far);
	
	void SetViewport(AABB2 const& viewport);
	void SetViewport(Vec2 const& viewportTopLeft, Vec2 const& viewportDimensions);
	void SetNormalizedViewport(Vec2 const& normalizedViewportTopLeftCoords, Vec2 const& normalizedViewportDimensions);
	void SetNormalizedViewport(Vec2 const& normalizedViewportTopLeftCoords, float normalizedViewportWidth, float normalizedViewportHeight);

	Vec2 GetOrthoBottomLeft() const { return m_orthoView.m_mins; };
	Vec2 GetOrthoTopRight() const { return m_orthoView.m_maxs; };
	void Translate2D(Vec2 const& translationXY);
	void Translate3D(Vec3 const& translationXYZ);
	void Rotate3D(EulerAngles const& rotationEulerAngles);
	void ClampPitch(float pitchLimit);
	void Zoom(float zoomScale);

	Mat44 GetModelMatrix() const;

	Mat44 GetOrthoMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetXRPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

	void SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis);
	Mat44 GetRenderMatrix() const;

	void SetTransform(Vec3 const& position, EulerAngles const& orientation);
	void SetTransform(Mat44 const& transform);
	Mat44 GetViewMatrix() const;

	Vec3 const GetPosition() const;
	EulerAngles const GetOrientation() const;

	AABB2 const GetViewport() const;
	Vec2 const GetNormalizedViewportBottomLeft() const;
	Vec2 const GetNormalizedViewportDimensions() const;
	Vec2 const GetNormalizedViewportTopRight() const;
	Vec2 const GetDirectXViewportTopLeft() const;
	float GetDirectXViewportWidth() const;
	float GetDirectXViewportHeight() const;

public:
	Mode m_mode = MODE_ORTHOGRAPHIC;

	AABB2 m_orthoView;
	float m_orthoNear;
	float m_orthoFar;

	float m_perspectiveAspect;
	float m_perspectiveFov;
	float m_perspectiveNear;
	float m_perspectiveFar;

	float m_xrAngleLeft;
	float m_xrAngleRight;
	float m_xrAngleUp;
	float m_xrAngleDown;
	float m_xrNear;
	float m_xrFar;

	Vec3 m_renderIBasis = Vec3::EAST;
	Vec3 m_renderJBasis = Vec3::NORTH;
	Vec3 m_renderKBasis = Vec3::SKYWARD;

	Vec3 m_position = Vec3::ZERO;
	EulerAngles m_orientation = EulerAngles::ZERO;
	Mat44 m_transform = Mat44::ZERO;

	Vec2 m_normalizedViewportBottomLeft = Vec2::ZERO;
	Vec2 m_normalizedViewportDimensions = -Vec2::ONE;

	AABB2 m_viewport = AABB2(Vec2(-1.f, -1.f), Vec2(-1.f, -1.f));
};
