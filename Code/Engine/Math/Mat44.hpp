#pragma once

#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Math/Vec4.hpp"

struct Vec2;
struct Vec3;


struct Mat44
{
public:
	enum {Ix, Iy, Iz, Iw,	Jx, Jy, Jz, Jw,		Kx, Ky, Kz, Kw,		Tx, Ty, Tz, Tw};
	float m_values[16];

public:
	Mat44();
	explicit Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D);
	explicit Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis, Vec3 const& translation3D);
	explicit Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D);
	explicit Mat44(float const* sixteenValuesBasisMajor);
	explicit Mat44(XmlElement const* xmlElement);

	static Mat44 const		CreateTranslation2D(Vec2 const& translationXY);
	static Mat44 const		CreateTranslation3D(Vec3 const& translationXYZ);
	static Mat44 const		CreateUniformScale2D(float uniformScaleXY);
	static Mat44 const		CreateUniformScale3D(float uniformScaleXYZ);
	static Mat44 const		CreateNonUniformScale2D(Vec2 const& nonUniformScaleXY);
	static Mat44 const		CreateNonUniformScale3D(Vec3 const& nonUniformScaleXYZ);
	static Mat44 const		CreateZRotationDegrees(float rotationDegreesAboutZ);
	static Mat44 const		CreateYRotationDegrees(float rotationDegreesAboutY);
	static Mat44 const		CreateXRotationDegrees(float rotationDegreesAboutX);
	static Mat44 const		CreateOrthoProjection(float xMin, float xMax, float yMin, float yMax, float orthoNear, float orthoFar);
	static Mat44 const		CreatePerspectiveProjection(float fov, float aspect, float perspectiveNear, float perspectiveFar);
	static Mat44 const		CreateOffCenterPersepectiveProjection(float angleLeft, float angleRight, float angleUp, float angleDown, float perspectiveNear, float perspectiveFar);

	Vec2 const				TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const;
	Vec3 const				TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const;
	Vec2 const				TransformPosition2D(Vec2 const& positionXY) const;
	Vec3 const				TransformPosition3D(Vec3 const& positionXYZ) const;
	Vec4 const				TransformHomogeneous3D(Vec4 const& homogenousPoint3D) const;

	float*					GetAsFloatArray();
	float const*			GetAsFloatArray() const;
	Vec2 const				GetIBasis2D() const;
	Vec2 const				GetJBasis2D() const;
	Vec2 const				GetTranslation2D() const;
	Vec3 const				GetIBasis3D() const;
	Vec3 const				GetJBasis3D() const;
	Vec3 const				GetKBasis3D() const;
	Vec3 const				GetTranslation3D() const;
	Vec4 const				GetIBasis4D() const;
	Vec4 const				GetJBasis4D() const;
	Vec4 const				GetKBasis4D() const;
	Vec4 const				GetTranslation4D() const;

	void					SetTranslation2D(Vec2 const& translationXY);
	void					SetTranslation3D(Vec3 const& translationXYZ);
	void					SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D);
	void					SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY);
	void					SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D);
	void					SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ);
	void					SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D);

	void					Append(Mat44 const& appendThis);
	void					AppendZRotation(float degreesRotationAboutZ);
	void					AppendYRotation(float degreesRotationY);
	void					AppendXRotation(float degreesRotationX);
	void					AppendTranslation2D(Vec2 const& translationXY);
	void					AppendTranslation3D(Vec3 const& translationXYZ);
	void					AppendScaleUniform2D(float uniformScaleXY);
	void					AppendScaleUniform3D(float uniformScaleXYZ);
	void					AppendScaleNonUniform2D(Vec2 const& nonUniformScaleXY);
	void					AppendScaleNonUniform3D(Vec3 const& nonUniformScaleXYZ);

	void					Transpose();
	Mat44 const				GetOrthonormalInverse() const;
	void					Orthonormalize_iFwd_jLeft_kUp();
	void Orthonormalize_iFwd_jLeft_kUp_PreserveK();

	bool operator!=(Mat44 const& matrixToCompare) const;

	static const Mat44 IDENTITY;
	static const Mat44 ZERO;
};

inline const Mat44 Mat44::IDENTITY{};
inline const Mat44 Mat44::ZERO{Vec4::ZERO, Vec4::ZERO, Vec4::ZERO, Vec4::ZERO};

typedef Mat44 Transform;
