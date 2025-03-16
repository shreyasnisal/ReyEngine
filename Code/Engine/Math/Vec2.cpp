#include "Engine/Math/Vec2.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"

#include <math.h>

Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{

}

Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}

void Vec2::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, ',');

	if (numSplitStrings != 2)
	{
		ERROR_AND_DIE("Incorrect number of literals in Vec2 string!");
	}

	x = static_cast<float>(atof(splitStrings[0].c_str()));
	y = static_cast<float>(atof(splitStrings[1].c_str()));
}

const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}

const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale );
}

const Vec2 Vec2::operator*(const Vec2& vecToMultiply) const
{
	return Vec2(x * vecToMultiply.x, y * vecToMultiply.y);
}

const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2(x / inverseScale, y / inverseScale);
}

void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}

void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}

bool Vec2::operator==( const Vec2& compare ) const
{
	return (x == compare.x && y == compare.y);
}

bool Vec2::operator!=( const Vec2& compare ) const
{
	return (x != compare.x || y != compare.y);
}

Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length)
{
	return Vec2(length * cosf(orientationRadians), length * sinf(orientationRadians));
}

Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length)
{
	return Vec2(length * CosDegrees(orientationDegrees), length * SinDegrees(orientationDegrees));
}

float Vec2::GetLength() const
{
	return sqrtf((x * x) + (y * y));
}

float Vec2::GetLengthSquared() const
{
	return ((x * x) + (y * y));
}

float Vec2::GetOrientationDegrees() const
{
	return Atan2Degrees(y, x);
}

float Vec2::GetOrientationRadians() const
{
	return atan2f(y, x);
}

Vec2 const Vec2::GetRotated90Degrees() const
{
	return Vec2(-y, x);
}

Vec2 const Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2(y, -x);
}

Vec2 const Vec2::GetRotatedRadians(float deltaRadians) const
{
	float length = GetLength();
	float orientationRadians = GetOrientationRadians();

	return Vec2(length * cosf(orientationRadians + deltaRadians), length * sinf(orientationRadians + deltaRadians));
}

Vec2 const Vec2::GetRotatedDegrees(float deltaDegrees) const
{
	float length = GetLength();
	float orientationDegrees = GetOrientationDegrees();

	return Vec2(length * CosDegrees(orientationDegrees + deltaDegrees), length * SinDegrees(orientationDegrees + deltaDegrees));
}

Vec2 const Vec2::GetClamped(float maxLength) const
{
	if (GetLength() > maxLength)
	{
		return (GetNormalized() * maxLength);
	}

	return Vec2(x, y);
}

Vec2 const Vec2::GetNormalized() const
{
	float length = sqrtf((x * x) + (y * y));
	if (length > 0)
	{
		return Vec2(x / length, y / length);
	}
	return Vec2();
}

Vec2 const Vec2::GetReflected(Vec2 const& surfaceNormal) const
{
	return *this - (2.f * DotProduct2D(*this, surfaceNormal) * surfaceNormal);
}

void Vec2::SetOrientationRadians(float newOrientationRadians)
{
	float length = GetLength();

	x = length * cosf(newOrientationRadians);
	y = length * sinf(newOrientationRadians);
}

void Vec2::SetOrientationDegrees(float newOrientationDegrees)
{
	float length = GetLength();
	x = length * CosDegrees(newOrientationDegrees);
	y = length * SinDegrees(newOrientationDegrees);
}

void Vec2::SetPolarRadians(float newOrientationRadians, float newLength)
{
	x = newLength * cosf(newOrientationRadians);
	y = newLength * sinf(newOrientationRadians);
}

void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength)
{
	x = newLength * CosDegrees(newOrientationDegrees);
	y = newLength * SinDegrees(newOrientationDegrees);
}

void Vec2::Rotate90Degrees()
{
	float tempX = x;
	x = -y;
	y = tempX;
}

void Vec2::RotateMinus90Degrees()
{
	float tempX = x;
	x = y;
	y = -tempX;
}

void Vec2::RotateRadians(float deltaRadians)
{
	float length = GetLength();
	float orientationRadians = GetOrientationRadians();
	x = length * cosf(orientationRadians + deltaRadians);
	y = length * sinf(orientationRadians + deltaRadians);
}

void Vec2::RotateDegrees(float deltaDegrees)
{
	float length = GetLength();
	float orientationDegrees = GetOrientationDegrees();
	x = length * CosDegrees(orientationDegrees + deltaDegrees);
	y = length * SinDegrees(orientationDegrees + deltaDegrees);
}

void Vec2::SetLength(float newLength)
{
	Normalize();
	x *= newLength;
	y *= newLength;
}

void Vec2::ClampLength(float maxLength)
{
	if (GetLength() > maxLength)
	{
		SetLength(maxLength);
	}
}

void Vec2::Normalize()
{
	float length = sqrtf((x * x) + (y * y));
	if (length > 0)
	{
		x = x / length;
		y = y / length;
	}
}

float Vec2::NormalizeAndGetPreviousLength()
{
	float length = sqrtf((x * x) + (y * y));
	if (length > 0)
	{
		x = x / length;
		y = y / length;
	}
	return length;
}

void Vec2::Reflect(Vec2 const& surfaceNormal)
{
	*this = *this - (2.f * DotProduct2D(*this, surfaceNormal) * surfaceNormal);
}

Vec3 const Vec2::ToVec3(float z) const
{
	return Vec3(x, y, z);
}
