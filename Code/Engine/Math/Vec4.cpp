#include "Engine/Math/Vec4.hpp"

#include <math.h>


Vec4::Vec4(float initialX, float initialY, float initialZ, float initialW)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
	, w(initialW)
{
}

Vec4 const Vec4::operator+(Vec4 const& vecToAdd) const
{
	return Vec4(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w);
}

void Vec4::operator+=(Vec4 const& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}

Vec4 const Vec4::operator-(Vec4 const& vecToSubtract) const
{
	return Vec4(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w);
}

void Vec4::operator-=(Vec4 const& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}

Vec4 const Vec4::operator*(float uniformScale) const
{
	return Vec4(x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale);
}

void Vec4::operator*=(float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

float Vec4::GetLength()
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

Vec4 const operator*(float uniformScale, Vec4 const& vecToScale)
{
	return Vec4(vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale, vecToScale.w * uniformScale);
}
