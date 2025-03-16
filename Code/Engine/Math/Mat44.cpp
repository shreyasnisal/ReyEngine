#include "Engine/Math/Mat44.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


Mat44::Mat44()
{
	m_values[Ix] = 1.f;
	m_values[Iy] = 0.f;
	m_values[Iz] = 0.f;
	m_values[Iw] = 0.f;

	m_values[Jx] = 0.f;
	m_values[Jy] = 1.f;
	m_values[Jz] = 0.f;
	m_values[Jw] = 0.f;

	m_values[Kx] = 0.f;
	m_values[Ky] = 0.f;
	m_values[Kz] = 1.f;
	m_values[Kw] = 0.f;

	m_values[Tx] = 0.f;
	m_values[Ty] = 0.f;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D)
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0.f;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0.f;
	m_values[Jw] = 0.f;

	m_values[Kx] = 0.f;
	m_values[Ky] = 0.f;
	m_values[Kz] = 1.f;
	m_values[Kw] = 0.f;

	m_values[Tx] = translation2D.x;
	m_values[Ty] = translation2D.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D)
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = 0.f;

	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = 0.f;

	m_values[Tx] = translation3D.x;
	m_values[Ty] = translation3D.y;
	m_values[Tz] = translation3D.z;
	m_values[Tw] = 1.f;
}

Mat44::Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_values[Ix] = iBasis4D.x;
	m_values[Iy] = iBasis4D.y;
	m_values[Iz] = iBasis4D.z;
	m_values[Iw] = iBasis4D.w;

	m_values[Jx] = jBasis4D.x;
	m_values[Jy] = jBasis4D.y;
	m_values[Jz] = jBasis4D.z;
	m_values[Jw] = jBasis4D.w;

	m_values[Kx] = kBasis4D.x;
	m_values[Ky] = kBasis4D.y;
	m_values[Kz] = kBasis4D.z;
	m_values[Kw] = kBasis4D.w;

	m_values[Tx] = translation4D.x;
	m_values[Ty] = translation4D.y;
	m_values[Tz] = translation4D.z;
	m_values[Tw] = translation4D.w;
}

Mat44::Mat44(float const* sixteenValuesBasisMajor)
{
	for (int i = 0; i < 16; i++)
	{
		m_values[i] = sixteenValuesBasisMajor[i];
	}
}

Mat44::Mat44(XmlElement const* xmlElement)
{
	Vec3 iBasis3D = ParseXmlAttribute(*xmlElement, "x", Vec3::EAST);
	Vec3 jBasis3D = ParseXmlAttribute(*xmlElement, "y", Vec3::NORTH);
	Vec3 kBasis3D = ParseXmlAttribute(*xmlElement, "z", Vec3::SKYWARD);
	Vec3 translation3D = ParseXmlAttribute(*xmlElement, "T", Vec3::ZERO);
	SetIJKT3D(iBasis3D, jBasis3D, kBasis3D, translation3D);
	float scale = ParseXmlAttribute(*xmlElement, "scale", 1.f);
	AppendScaleUniform3D(scale);
}

Mat44 const Mat44::CreateTranslation2D(Vec2 const& translationXY)
{
	return Mat44(Vec2::EAST, Vec2::NORTH, translationXY);
}

Mat44 const Mat44::CreateTranslation3D(Vec3 const& translationXYZ)
{
	return Mat44(Vec3::EAST, Vec3::NORTH, Vec3::SKYWARD, translationXYZ);
}

Mat44 const Mat44::CreateUniformScale2D(float uniformScaleXY)
{
	Mat44 result = Mat44();
	result.m_values[Ix] = uniformScaleXY;
	result.m_values[Jy] = uniformScaleXY;
	return result;
}

Mat44 const Mat44::CreateUniformScale3D(float uniformScaleXYZ)
{
	Mat44 result = Mat44();
	result.m_values[Ix] = uniformScaleXYZ;
	result.m_values[Jy] = uniformScaleXYZ;
	result.m_values[Kz] = uniformScaleXYZ;
	return result;
}

Mat44 const Mat44::CreateNonUniformScale2D(Vec2 const& nonUniformScaleXY)
{
	Mat44 result = Mat44();
	result.m_values[Ix] = nonUniformScaleXY.x;
	result.m_values[Jy] = nonUniformScaleXY.y;
	return result;
}

Mat44 const Mat44::CreateNonUniformScale3D(Vec3 const& nonUniformScaleXYZ)
{
	Mat44 result = Mat44();
	result.m_values[Ix] = nonUniformScaleXYZ.x;
	result.m_values[Jy] = nonUniformScaleXYZ.y;
	result.m_values[Kz] = nonUniformScaleXYZ.z;
	return result;
}

Mat44 const Mat44::CreateZRotationDegrees(float rotationDegreesAboutZ)
{
	Mat44 result = Mat44();
	result.m_values[Ix] = CosDegrees(rotationDegreesAboutZ);
	result.m_values[Iy] = SinDegrees(rotationDegreesAboutZ);
	result.m_values[Jx] = -SinDegrees(rotationDegreesAboutZ);
	result.m_values[Jy] = CosDegrees(rotationDegreesAboutZ);
	return result;
}

Mat44 const Mat44::CreateYRotationDegrees(float rotationDegreesAboutY)
{
	Mat44 result = Mat44();
	result.m_values[Ix] = CosDegrees(rotationDegreesAboutY);
	result.m_values[Iz] = -SinDegrees(rotationDegreesAboutY);
	result.m_values[Kx] = SinDegrees(rotationDegreesAboutY);
	result.m_values[Kz] = CosDegrees(rotationDegreesAboutY);
	return result;
}

Mat44 const Mat44::CreateXRotationDegrees(float rotationDegreesAboutX)
{
	Mat44 result = Mat44();
	result.m_values[Jy] = CosDegrees(rotationDegreesAboutX);
	result.m_values[Jz] = SinDegrees(rotationDegreesAboutX);
	result.m_values[Ky] = -SinDegrees(rotationDegreesAboutX);
	result.m_values[Kz] = CosDegrees(rotationDegreesAboutX);
	return result;
}

Mat44 const Mat44::CreateOrthoProjection(float xMin, float xMax, float yMin, float yMax, float orthoNear, float orthoFar)
{
	Mat44 orthoMatrix = Mat44();

	float orthoRight = xMax;
	float orthoLeft = xMin;
	float orthoTop = yMax;
	float orthoBottom = yMin;

	orthoMatrix.m_values[Mat44::Ix] = 2.f / (orthoRight - orthoLeft);
	orthoMatrix.m_values[Mat44::Jy] = 2.f / (orthoTop - orthoBottom);
	orthoMatrix.m_values[Mat44::Kz] = 1.f / (orthoFar - orthoNear);

	orthoMatrix.m_values[Mat44::Tx] = -(orthoRight + orthoLeft) / (orthoRight - orthoLeft);
	orthoMatrix.m_values[Mat44::Ty] = -(orthoTop + orthoBottom) / (orthoTop - orthoBottom);
	orthoMatrix.m_values[Mat44::Tz] = -orthoNear / (orthoFar - orthoNear);

	return orthoMatrix;
}

Mat44 const Mat44::CreatePerspectiveProjection(float fov, float aspect, float perspectiveNear, float perspectiveFar)
{
	float top = perspectiveNear * TanDegrees(fov * 0.5f);
	float bottom = -top;
	float right = top * aspect;
	float left = -right;

	Mat44 perspectiveMatrix = Mat44();

	if (perspectiveNear == 0.f)
	{
		perspectiveMatrix.m_values[Mat44::Ix] = 1.f;
		perspectiveMatrix.m_values[Mat44::Jy] = 1.f;
	}
	else
	{
		perspectiveMatrix.m_values[Mat44::Ix] = 2.f * perspectiveNear / (right - left);
		perspectiveMatrix.m_values[Mat44::Jy] = 2.f * perspectiveNear / (top - bottom);
	}
	perspectiveMatrix.m_values[Mat44::Kz] = (perspectiveFar) / (perspectiveFar - perspectiveNear);
	perspectiveMatrix.m_values[Mat44::Kw] = 1.f;

	perspectiveMatrix.m_values[Mat44::Tz] = -1.f * perspectiveFar * perspectiveNear / (perspectiveFar - perspectiveNear);
	perspectiveMatrix.m_values[Mat44::Tw] = 0.f;

	return perspectiveMatrix;
}

Mat44 const Mat44::CreateOffCenterPersepectiveProjection(float angleLeft, float angleRight, float angleUp, float angleDown, float perspectiveNear, float perspectiveFar)
{
	float tanLeft = tanf(angleLeft);
	float tanRight = tanf(angleRight);
	float tanUp = tanf(angleUp);
	float tanDown = tanf(angleDown);

	float left = perspectiveNear * tanLeft;
	float right = perspectiveNear * tanRight;
	float top = perspectiveNear * tanUp;
	float bottom = perspectiveNear * tanDown;

	Mat44 offCenterPespectiveMatrix = Mat44();

	offCenterPespectiveMatrix.m_values[Mat44::Ix] = 2.f * perspectiveNear / (right - left);
	offCenterPespectiveMatrix.m_values[Mat44::Jy] = 2.f * perspectiveNear / (top - bottom);

	offCenterPespectiveMatrix.m_values[Mat44::Kx] = (left + right) / (right - left);
	offCenterPespectiveMatrix.m_values[Mat44::Ky] = (top + bottom) / (top - bottom);
	offCenterPespectiveMatrix.m_values[Mat44::Kz] = perspectiveFar / (perspectiveNear - perspectiveFar);
	offCenterPespectiveMatrix.m_values[Mat44::Kw] = -1.f;

	offCenterPespectiveMatrix.m_values[Mat44::Tz] = -1.f * perspectiveFar * perspectiveNear / (perspectiveFar - perspectiveNear);
	offCenterPespectiveMatrix.m_values[Mat44::Tw] = 0.f;

	return offCenterPespectiveMatrix;
}

Vec2 const Mat44::TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const
{
	Vec2 result;

	result.x = DotProduct4D(Vec4(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]), Vec4(vectorQuantityXY.x, vectorQuantityXY.y, 0.f, 0.f));
	result.y = DotProduct4D(Vec4(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]), Vec4(vectorQuantityXY.x, vectorQuantityXY.y, 0.f, 0.f));

	return result;
}

Vec3 const Mat44::TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const
{
	Vec3 result;

	result.x = DotProduct4D(Vec4(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]), Vec4(vectorQuantityXYZ.x, vectorQuantityXYZ.y, vectorQuantityXYZ.z, 0.f));
	result.y = DotProduct4D(Vec4(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]), Vec4(vectorQuantityXYZ.x, vectorQuantityXYZ.y, vectorQuantityXYZ.z, 0.f));
	result.z = DotProduct4D(Vec4(m_values[Iz], m_values[Jz], m_values[Kz], m_values[Tz]), Vec4(vectorQuantityXYZ.x, vectorQuantityXYZ.y, vectorQuantityXYZ.z, 0.f));

	return result;
}

Vec2 const Mat44::TransformPosition2D(Vec2 const& positionXY) const
{
	Vec2 result;

	result.x = DotProduct4D(Vec4(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]), Vec4(positionXY.x, positionXY.y, 0.f, 1.f));
	result.y = DotProduct4D(Vec4(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]), Vec4(positionXY.x, positionXY.y, 0.f, 1.f));

	return result;
}

Vec3 const Mat44::TransformPosition3D(Vec3 const& positionXYZ) const
{
	Vec3 result;

	result.x = DotProduct4D(Vec4(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]), Vec4(positionXYZ.x, positionXYZ.y, positionXYZ.z, 1.f));
	result.y = DotProduct4D(Vec4(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]), Vec4(positionXYZ.x, positionXYZ.y, positionXYZ.z, 1.f));
	result.z = DotProduct4D(Vec4(m_values[Iz], m_values[Jz], m_values[Kz], m_values[Tz]), Vec4(positionXYZ.x, positionXYZ.y, positionXYZ.z, 1.f));

	return result;
}

Vec4 const Mat44::TransformHomogeneous3D(Vec4 const& homogenousPoint3D) const
{
	Vec4 result;

	result.x = DotProduct4D(Vec4(m_values[Ix], m_values[Jx], m_values[Kx], m_values[Tx]), Vec4(homogenousPoint3D.x, homogenousPoint3D.y, homogenousPoint3D.z, homogenousPoint3D.w));
	result.y = DotProduct4D(Vec4(m_values[Iy], m_values[Jy], m_values[Ky], m_values[Ty]), Vec4(homogenousPoint3D.x, homogenousPoint3D.y, homogenousPoint3D.z, homogenousPoint3D.w));
	result.z = DotProduct4D(Vec4(m_values[Iz], m_values[Jz], m_values[Kz], m_values[Tz]), Vec4(homogenousPoint3D.x, homogenousPoint3D.y, homogenousPoint3D.z, homogenousPoint3D.w));
	result.w = DotProduct4D(Vec4(m_values[Iw], m_values[Jw], m_values[Kw], m_values[Tw]), Vec4(homogenousPoint3D.x, homogenousPoint3D.y, homogenousPoint3D.z, homogenousPoint3D.w));

	return result;
}

float* Mat44::GetAsFloatArray()
{
	return m_values;
}

float const* Mat44::GetAsFloatArray() const
{
	return m_values;
}

Vec2 const Mat44::GetIBasis2D() const
{
	return Vec2(m_values[Ix], m_values[Iy]);
}

Vec2 const Mat44::GetJBasis2D() const
{
	return Vec2(m_values[Jx], m_values[Jy]);
}

Vec2 const Mat44::GetTranslation2D() const
{
	return Vec2(m_values[Tx], m_values[Ty]);
}

Vec3 const Mat44::GetIBasis3D() const
{
	return Vec3(m_values[Ix], m_values[Iy], m_values[Iz]);
}

Vec3 const Mat44::GetJBasis3D() const
{
	return Vec3(m_values[Jx], m_values[Jy], m_values[Jz]);
}

Vec3 const Mat44::GetKBasis3D() const
{
	return Vec3(m_values[Kx], m_values[Ky], m_values[Kz]);
}

Vec3 const Mat44::GetTranslation3D() const
{
	return Vec3(m_values[Tx], m_values[Ty], m_values[Tz]);
}

Vec4 const Mat44::GetIBasis4D() const
{
	return Vec4(m_values[Ix], m_values[Iy], m_values[Iz], m_values[Iw]);
}

Vec4 const Mat44::GetJBasis4D() const
{
	return Vec4(m_values[Jx], m_values[Jy], m_values[Jz], m_values[Jw]);
}

Vec4 const Mat44::GetKBasis4D() const
{
	return Vec4(m_values[Kx], m_values[Ky], m_values[Kz], m_values[Kw]);
}

Vec4 const Mat44::GetTranslation4D() const
{
	return Vec4(m_values[Tx], m_values[Ty], m_values[Tz], m_values[Tw]);
}

void Mat44::SetTranslation2D(Vec2 const& translationXY)
{
	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

void Mat44::SetTranslation3D(Vec3 const& translationXYZ)
{
	m_values[Tx] = translationXYZ.x;
	m_values[Ty] = translationXYZ.y;
	m_values[Tz] = translationXYZ.z;
	m_values[Tw] = 1.f;
}

void Mat44::SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D)
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0.f;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0.f;
	m_values[Jw] = 0.f;
}

void Mat44::SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY)
{
	m_values[Ix] = iBasis2D.x;
	m_values[Iy] = iBasis2D.y;
	m_values[Iz] = 0.f;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis2D.x;
	m_values[Jy] = jBasis2D.y;
	m_values[Jz] = 0.f;
	m_values[Jw] = 0.f;

	m_values[Tx] = translationXY.x;
	m_values[Ty] = translationXY.y;
	m_values[Tz] = 0.f;
	m_values[Tw] = 1.f;
}

void Mat44::SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D)
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = 0.f;

	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = 0.f;
}

void Mat44::SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ)
{
	m_values[Ix] = iBasis3D.x;
	m_values[Iy] = iBasis3D.y;
	m_values[Iz] = iBasis3D.z;
	m_values[Iw] = 0.f;

	m_values[Jx] = jBasis3D.x;
	m_values[Jy] = jBasis3D.y;
	m_values[Jz] = jBasis3D.z;
	m_values[Jw] = 0.f;

	m_values[Kx] = kBasis3D.x;
	m_values[Ky] = kBasis3D.y;
	m_values[Kz] = kBasis3D.z;
	m_values[Kw] = 0.f;

	m_values[Tx] = translationXYZ.x;
	m_values[Ty] = translationXYZ.y;
	m_values[Tz] = translationXYZ.z;
	m_values[Tw] = 1.f;
}

void Mat44::SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_values[Ix] = iBasis4D.x;
	m_values[Iy] = iBasis4D.y;
	m_values[Iz] = iBasis4D.z;
	m_values[Iw] = iBasis4D.w;

	m_values[Jx] = jBasis4D.x;
	m_values[Jy] = jBasis4D.y;
	m_values[Jz] = jBasis4D.z;
	m_values[Jw] = jBasis4D.w;

	m_values[Kx] = kBasis4D.x;
	m_values[Ky] = kBasis4D.y;
	m_values[Kz] = kBasis4D.z;
	m_values[Kw] = kBasis4D.w;

	m_values[Tx] = translation4D.x;
	m_values[Ty] = translation4D.y;
	m_values[Tz] = translation4D.z;
	m_values[Tw] = translation4D.w;
}

void Mat44::Append(Mat44 const& appendThis)
{
	Mat44 result;
	for (int rowIndex = 0; rowIndex < 4; rowIndex++)
	{
		for (int colIndex = 0; colIndex < 4; colIndex++)
		{
			int resultIndex = rowIndex + colIndex * 4;
			Vec4 matrixRow(m_values[rowIndex + 0], m_values[rowIndex + 4], m_values[rowIndex + 8], m_values[rowIndex + 12]);
			Vec4 appendMatrixCol(appendThis.m_values[colIndex * 4], appendThis.m_values[colIndex * 4 + 1], appendThis.m_values[colIndex * 4 + 2], appendThis.m_values[colIndex * 4 + 3]);
			result.m_values[resultIndex] = DotProduct4D(matrixRow, appendMatrixCol);
		}
	}

	for (int valueIndex = 0; valueIndex < 16; valueIndex++)
	{
		m_values[valueIndex] = result.m_values[valueIndex];
	}
}

void Mat44::AppendZRotation(float degreesRotationAboutZ)
{
	Mat44 zRotationMatrix = Mat44::CreateZRotationDegrees(degreesRotationAboutZ);
	Append(zRotationMatrix);
}

void Mat44::AppendYRotation(float degreesRotationY)
{
	Mat44 yRotationMatrix = Mat44::CreateYRotationDegrees(degreesRotationY);
	Append(yRotationMatrix);
}

void Mat44::AppendXRotation(float degreesRotationX)
{
	Mat44 xRotationMatrix = Mat44::CreateXRotationDegrees(degreesRotationX);
	Append(xRotationMatrix);
}

void Mat44::AppendTranslation2D(Vec2 const& translationXY)
{
	Mat44 translationMatrix = Mat44::CreateTranslation2D(translationXY);
	Append(translationMatrix);
}

void Mat44::AppendTranslation3D(Vec3 const& translationXYZ)
{
	Mat44 translationMatrix = Mat44::CreateTranslation3D(translationXYZ);
	Append(translationMatrix);
}

void Mat44::AppendScaleUniform2D(float uniformScaleXY)
{
	Mat44 uniformScaleMatrix = Mat44::CreateUniformScale2D(uniformScaleXY);
	Append(uniformScaleMatrix);
}

void Mat44::AppendScaleUniform3D(float uniformScaleXYZ)
{
	Mat44 uniformScaleMatrix = Mat44::CreateUniformScale3D(uniformScaleXYZ);
	Append(uniformScaleMatrix);
}

void Mat44::AppendScaleNonUniform2D(Vec2 const& nonUniformScaleXY)
{
	Mat44 nonUniformScaleMatrix = Mat44::CreateNonUniformScale2D(nonUniformScaleXY);
	Append(nonUniformScaleMatrix);
}

void Mat44::AppendScaleNonUniform3D(Vec3 const& nonUniformScaleXYZ)
{
	Mat44 nonUniformScaleMatrix = Mat44::CreateNonUniformScale3D(nonUniformScaleXYZ);
	Append(nonUniformScaleMatrix);
}

void Mat44::Transpose()
{
	Mat44 matrixCopy(*this);

	m_values[Iy] = matrixCopy.m_values[Jx];
	m_values[Iz] = matrixCopy.m_values[Kx];
	m_values[Iw] = matrixCopy.m_values[Tx];

	m_values[Jx] = matrixCopy.m_values[Iy];
	m_values[Jz] = matrixCopy.m_values[Ky];
	m_values[Jw] = matrixCopy.m_values[Ty];

	m_values[Kx] = matrixCopy.m_values[Iz];
	m_values[Ky] = matrixCopy.m_values[Jz];
	m_values[Kw] = matrixCopy.m_values[Tz];

	m_values[Tx] = matrixCopy.m_values[Iw];
	m_values[Ty] = matrixCopy.m_values[Jw];
	m_values[Tz] = matrixCopy.m_values[Kw];
}

Mat44 const Mat44::GetOrthonormalInverse() const
{
	Mat44 rotationInverseMatrix(GetIBasis3D(), GetJBasis3D(), GetKBasis3D(), Vec3::ZERO);
	rotationInverseMatrix.Transpose();

	Mat44 inverseMatrix = rotationInverseMatrix;
	inverseMatrix.AppendTranslation3D(-GetTranslation3D());

	return inverseMatrix;
}

void Mat44::Orthonormalize_iFwd_jLeft_kUp()
{
	Vec3 iBasis = GetIBasis3D();
	Vec3 jBasis = GetJBasis3D();
	Vec3 kBasis = GetKBasis3D();

	Vec3 iOrthoNormalized = iBasis.GetNormalized();

	Vec3 kBad = DotProduct3D(iOrthoNormalized, kBasis) * iOrthoNormalized;
	Vec3 kOrtho = kBasis - kBad;
	Vec3 kOrthoNormalized = kOrtho.GetNormalized();

	Vec3 jBadAlongI = DotProduct3D(iOrthoNormalized, jBasis) * iOrthoNormalized;
	Vec3 jBadAlongK = DotProduct3D(kOrthoNormalized, jBasis) * kOrthoNormalized;
	Vec3 jOrtho = jBasis - (jBadAlongI + jBadAlongK);
	Vec3 jOrthoNormalized = jOrtho.GetNormalized();

	SetIJKT3D(iOrthoNormalized, jOrthoNormalized, kOrthoNormalized, GetTranslation3D());
}


void Mat44::Orthonormalize_iFwd_jLeft_kUp_PreserveK()
{
	Vec3 iBasis = GetIBasis3D();
	Vec3 jBasis = GetJBasis3D();
	Vec3 kBasis = GetKBasis3D();

	Vec3 kOrthoNormalized = kBasis.GetNormalized();

	Vec3 iBad = DotProduct3D(kOrthoNormalized, iBasis) * kOrthoNormalized;
	Vec3 iOrtho = iBasis - iBad;
	Vec3 iOrthoNormalized = iOrtho.GetNormalized();

	Vec3 jBadAlongI = DotProduct3D(iOrthoNormalized, jBasis) * iOrthoNormalized;
	Vec3 jBadAlongK = DotProduct3D(kOrthoNormalized, jBasis) * kOrthoNormalized;
	Vec3 jOrtho = jBasis - (jBadAlongI + jBadAlongK);
	Vec3 jOrthoNormalized = jOrtho.GetNormalized();

	SetIJKT3D(iOrthoNormalized, jOrthoNormalized, kOrthoNormalized, GetTranslation3D());
}

bool Mat44::operator!=(Mat44 const& matrixToCompare) const
{
	for (int i = 0; i < 16; i++)
	{
		if (m_values[i] != matrixToCompare.m_values[i])
		{
			return true;
		}
	}

	return false;
}
