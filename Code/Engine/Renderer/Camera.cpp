#include "Engine/Renderer/Camera.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Window.hpp"

void Camera::SetOrthoView(const Vec2& bottomLeft, const Vec2& topRight, float near, float far)
{
	m_orthoView.m_mins = bottomLeft;
	m_orthoView.m_maxs = topRight;
	m_orthoNear = near;
	m_orthoFar = far;

	m_mode = MODE_ORTHOGRAPHIC;
}

void Camera::SetPerspectiveView(float aspect, float fov, float near, float far)
{
	m_perspectiveAspect = aspect;
	m_perspectiveFov = fov;
	m_perspectiveNear = near;
	m_perspectiveFar = far;

	m_mode = MODE_PERSPECTIVE;
}

void Camera::SetXRView(float angleLeft, float angleRight, float angleUp, float angleDown, float near, float far)
{
	m_xrAngleLeft = angleLeft;
	m_xrAngleRight = angleRight;
	m_xrAngleUp = angleUp;
	m_xrAngleDown = angleDown;
	m_xrNear = near;
	m_xrFar = far;

	m_mode = MODE_XR;
}

void Camera::SetViewport(AABB2 const& viewport)
{
	m_viewport = viewport;
}

void Camera::SetViewport(Vec2 const& viewportTopLeft, Vec2 const& viewportDimensions)
{
	m_viewport.m_mins = viewportTopLeft - Vec2 (0.f, viewportDimensions.y);
	m_viewport.m_maxs = viewportTopLeft + Vec2(viewportDimensions.x, 0.f);
}

void Camera::SetNormalizedViewport(Vec2 const& normalizedViewportBottomLeft, Vec2 const& normalizedViewportDimensions)
{
	m_normalizedViewportBottomLeft = normalizedViewportBottomLeft;
	m_normalizedViewportDimensions = normalizedViewportDimensions;
}

void Camera::SetNormalizedViewport(Vec2 const& normalizedViewportBottomLeft, float normalizedViewportWidth, float normalizedViewportHeight)
{
	m_normalizedViewportBottomLeft = normalizedViewportBottomLeft;
	m_normalizedViewportDimensions = Vec2(normalizedViewportWidth, normalizedViewportHeight);
}

void Camera::Translate2D(Vec2 const& translationXY)
{
	m_orthoView.SetCenter(m_orthoView.GetCenter() + translationXY);
}

void Camera::Translate3D(Vec3 const& translationXYZ)
{
	m_position += translationXYZ;
}

void Camera::Rotate3D(EulerAngles const& rotationEulerAngles)
{
	m_orientation.m_yawDegrees += rotationEulerAngles.m_yawDegrees;
	m_orientation.m_pitchDegrees += rotationEulerAngles.m_pitchDegrees;
	m_orientation.m_rollDegrees += rotationEulerAngles.m_rollDegrees;
}

void Camera::ClampPitch(float pitchLimit)
{
	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -pitchLimit, pitchLimit);
}

void Camera::Zoom(float zoomScale)
{
	float dimensionScale = 1.f / zoomScale;

	m_orthoView.SetDimensions(m_orthoView.GetDimensions() * dimensionScale);
}

Mat44 Camera::GetModelMatrix() const
{
	if (m_transform != Mat44::ZERO)
	{
		return m_transform;
	}

	Mat44 modelMatrix = Mat44::CreateTranslation3D(m_position);
	modelMatrix.Append(m_orientation.GetAsMatrix_iFwd_jLeft_kUp());
	return modelMatrix;
}

Mat44 Camera::GetOrthoMatrix() const
{
	return Mat44::CreateOrthoProjection(m_orthoView.m_mins.x, m_orthoView.m_maxs.x, m_orthoView.m_mins.y, m_orthoView.m_maxs.y, m_orthoNear, m_orthoFar);
}

Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::CreatePerspectiveProjection(m_perspectiveFov, m_perspectiveAspect, m_perspectiveNear, m_perspectiveFar);
}

Mat44 Camera::GetXRPerspectiveMatrix() const
{
	return Mat44::CreateOffCenterPersepectiveProjection(m_xrAngleLeft, m_xrAngleRight, m_xrAngleUp, m_xrAngleDown, m_xrNear, m_xrFar);
}

Mat44 Camera::GetProjectionMatrix() const
{
	Mat44 projectionMatrix;

	if (m_mode == MODE_PERSPECTIVE)
	{
		projectionMatrix = GetPerspectiveMatrix();
	}
	else if (m_mode == MODE_XR)
	{
		projectionMatrix = GetXRPerspectiveMatrix();
	}
	else
	{
		projectionMatrix = GetOrthoMatrix();
	}

	Mat44 renderMatrix = GetRenderMatrix();
	projectionMatrix.Append(renderMatrix);
	return projectionMatrix;
}

void Camera::SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis)
{
	m_renderIBasis = iBasis;
	m_renderJBasis = jBasis;
	m_renderKBasis = kBasis;
}

Mat44 Camera::GetRenderMatrix() const
{
	return Mat44(m_renderIBasis, m_renderJBasis, m_renderKBasis, Vec3::ZERO);
}

void Camera::SetTransform(Vec3 const& position, EulerAngles const& orientation)
{
	m_position = position;
	m_orientation = orientation;
}

void Camera::SetTransform(Mat44 const& transform)
{
	m_transform = transform;
}

Mat44 Camera::GetViewMatrix() const
{
	if (m_transform != Mat44::ZERO)
	{
		return m_transform.GetOrthonormalInverse();
	}

	Mat44 lookAtMatrix = Mat44::CreateTranslation3D(m_position);
	lookAtMatrix.Append(m_orientation.GetAsMatrix_iFwd_jLeft_kUp());
	Mat44 viewMatrix = lookAtMatrix.GetOrthonormalInverse();
	return viewMatrix;
}

Vec3 const Camera::GetPosition() const
{
	return m_position;
}

EulerAngles const Camera::GetOrientation() const
{
	if (m_transform != Mat44::ZERO)
	{
		float yaw = 0.f;
		float pitch = 0.f;
		float roll = 0.f;
		enum {Ix, Iy, Iz, Iw,	Jx, Jy, Jz, Jw,		Kx, Ky, Kz, Kw,		Tx, Ty, Tz, Tw};

		if (m_transform.m_values[Iz] <= -1.0f)
		{
			pitch = 90.f;
			yaw = Atan2Degrees(-m_transform.m_values[Jx], m_transform.m_values[Ix]);
			roll = 0.0f;
		}
		else if (m_transform.m_values[Iz] >= 1.0f)
		{
			pitch = -90.f;
			yaw = Atan2Degrees(-m_transform.m_values[Jx], m_transform.m_values[Ix]);
			roll = 0.0f;
		}
		else
		{
			pitch = AsinDegrees(-m_transform.m_values[Iz]);
			yaw = Atan2Degrees(m_transform.m_values[Iy], m_transform.m_values[Ix]);
			roll = Atan2Degrees(m_transform.m_values[Jz], m_transform.m_values[Kz]);
		}

		return EulerAngles(yaw, pitch, roll);
	}

	return m_orientation;
}

AABB2 const Camera::GetViewport() const
{
	return m_viewport;
}

Vec2 const Camera::GetNormalizedViewportBottomLeft() const
{
	return Vec2(m_normalizedViewportBottomLeft.x, m_normalizedViewportBottomLeft.y);
}

Vec2 const Camera::GetNormalizedViewportTopRight() const
{
	return m_normalizedViewportBottomLeft + m_normalizedViewportDimensions;
}

Vec2 const Camera::GetNormalizedViewportDimensions() const
{
	return m_normalizedViewportDimensions;
}

float Camera::GetDirectXViewportWidth() const
{
	return m_normalizedViewportDimensions.x * Window::GetMainWindowInstance()->GetClientDimensions().x;
}

float Camera::GetDirectXViewportHeight() const
{
	return m_normalizedViewportDimensions.y * Window::GetMainWindowInstance()->GetClientDimensions().y;
}

Vec2 const Camera::GetDirectXViewportTopLeft() const
{
	if (m_normalizedViewportDimensions.x == -1.f && m_normalizedViewportDimensions.y == -1.f)
	{
		return Vec2::ZERO;
	}

	Vec2 clientDimensions = Window::GetMainWindowInstance()->GetClientDimensions().GetAsVec2();
	return Vec2(m_normalizedViewportBottomLeft.x, (1.f - m_normalizedViewportBottomLeft.y - m_normalizedViewportDimensions.y)) * clientDimensions;
}
