#include "Engine/Math/Vec3.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"

#include <math.h>

Vec3::Vec3(const Vec3& copyFrom)
	: x(copyFrom.x)
	, y(copyFrom.y)
	, z(copyFrom.z)
{

}

Vec3::Vec3(float initialX, float initialY, float initialZ)
	: x(initialX)
	, y(initialY)
	, z(initialZ)
{

}

// #TODO Fix this!
Vec3 const Vec3::MakeFromPolarRadians(float latitudeRadians, float longitudeRadians, float length)
{
	return Vec3(length * cosf(latitudeRadians) * sinf(longitudeRadians), length * sinf(latitudeRadians) * sinf(longitudeRadians), length * cosf(longitudeRadians));
}

Vec3 const Vec3::MakeFromPolarDegrees(float yawDegrees, float pitchDegrees, float length)
{
	return Vec3(length * CosDegrees(yawDegrees) * CosDegrees(pitchDegrees), length * SinDegrees(yawDegrees) * CosDegrees(pitchDegrees), -length * SinDegrees(pitchDegrees));
}

Vec3 const Vec3::MakeFromCylindricalRadians(float radialDistance, float azimuthRadians, float z)
{
	return Vec3(radialDistance * cosf(azimuthRadians), radialDistance * sinf(azimuthRadians), z);
}

Vec3 const Vec3::MakeFromCylindricalDegrees(float radialDistance, float azimuthDegrees, float z)
{
	return Vec3(radialDistance * CosDegrees(azimuthDegrees), radialDistance * SinDegrees(azimuthDegrees), z);
}

void Vec3::SetFromText(char const* text)
{
	Strings splitStrings;
	std::string textStr = text;
	StripString(textStr, ' ');
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, textStr, ',', true);

	if (numSplitStrings != 3)
	{
		ERROR_AND_DIE("Incorrect number of literals in Vec3 string!");
	}

	x = static_cast<float>(atof(splitStrings[0].c_str()));
	y = static_cast<float>(atof(splitStrings[1].c_str()));
	z = static_cast<float>(atof(splitStrings[2].c_str()));
}

const Vec3 Vec3::operator+(const Vec3& vecToAdd) const
{
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

const Vec3 Vec3::operator-(const Vec3& vecToSubtract) const
{
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

const Vec3 Vec3::operator-() const
{
	return Vec3(-x, -y, -z);
}

const Vec3 Vec3::operator*(float uniformScale) const
{
	return Vec3(x * uniformScale, y * uniformScale, z * uniformScale);
}

const Vec3 Vec3::operator*(const Vec3& vecToMultiply) const
{
	return Vec3(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z);
}

const Vec3 Vec3::operator/(float inverseScale) const
{
	return Vec3(x / inverseScale, y / inverseScale, z / inverseScale);
}

void Vec3::operator+=(const Vec3& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

void Vec3::operator-=(const Vec3& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

void Vec3::operator*=(const Vec3& vecToMultiply)
{
	x *= vecToMultiply.x;
	y *= vecToMultiply.y;
	z *= vecToMultiply.z;
}

void Vec3::operator*=(const float uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

void Vec3::operator/=(const float uniformDivisor)
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}

void Vec3::operator=(const Vec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

const Vec3 operator*(float uniformScale, const Vec3 vecToScale)
{
	return Vec3(vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);
}

bool Vec3::operator==(const Vec3& compare) const
{
	return (x == compare.x && y == compare.y && z == compare.z);
}

bool Vec3::operator!=(const Vec3& compare) const
{
	return (x != compare.x || y != compare.y || z != compare.z);
}

const Vec3 operator*(float uniformScale, const Vec3& vecToScale)
{
	return Vec3(vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);
}

float Vec3::GetLength() const
{
	return sqrtf((x * x) + (y * y) + (z * z));
}

float Vec3::GetLengthXY() const
{
	return sqrtf((x * x) + (y * y));
}

float Vec3::GetLengthYZ() const
{
	return sqrtf(y * y + z * z);
}

float Vec3::GetLengthZX() const
{
	return sqrtf(z * z + x * x);
}

float Vec3::GetLengthSquared() const
{
	return ((x * x) + (y * y) + (z * z));
}

float Vec3::GetLengthXYSquared() const
{
	return ((x * x) + (y * y));
}

float Vec3::GetLengthYZSquared() const
{
	return (y * y + z * z);
}

float Vec3::GetLengthZXSquared() const
{
	return (z * z + x * x);
}

float Vec3::GetAngleAboutZRadians() const
{
	return atan2f(y, x);
}

float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees(y, x);
}

float Vec3::GetAngleAboutYRadians() const
{
	return atan2f(x, z);
}

float Vec3::GetAngleAboutYDegrees() const
{
	return Atan2Degrees(x, z);
}

float Vec3::GetAngleAboutXRadians() const
{
	return atan2f(z, y);
}

float Vec3::GetAngleAboutXDegrees() const
{
	return Atan2Degrees(z, y);
}

Vec3 const Vec3::GetRotatedAboutZRadians(float deltaRadians) const
{
	float lengthXY = GetLengthXY();
	float angleAboutZRadians = GetAngleAboutZRadians();
	return Vec3(lengthXY * cosf(angleAboutZRadians + deltaRadians), lengthXY * sinf(angleAboutZRadians + deltaRadians), z);
}

Vec3 const Vec3::GetRotatedAboutZDegrees(float deltaDegrees) const
{
	float lengthXY = GetLengthXY();
	float angleAboutZDegrees = GetAngleAboutZDegrees();
	return Vec3(lengthXY * CosDegrees(angleAboutZDegrees + deltaDegrees), lengthXY * SinDegrees(angleAboutZDegrees + deltaDegrees), z);
}

Vec3 const Vec3::GetRotatedAboutYRadians(float deltaRadians) const
{
	float lengthZX = GetLengthZX();
	float angleAboutYRadians = GetAngleAboutYRadians();
	return Vec3(lengthZX * sinf(angleAboutYRadians + deltaRadians), y, lengthZX * cosf(angleAboutYRadians + deltaRadians));
}

Vec3 const Vec3::GetRotatedAboutYDegrees(float deltaDegrees) const
{
	float lengthZX = GetLengthZX();
	float angleAboutYDegrees = GetAngleAboutYDegrees();
	return Vec3(lengthZX * SinDegrees(angleAboutYDegrees + deltaDegrees), y, lengthZX * CosDegrees(angleAboutYDegrees + deltaDegrees));
}

Vec3 const Vec3::GetRotatedAboutXRadians(float deltaRadians) const
{
	float lengthYZ = GetLengthYZ();
	float angleAboutXRadians = GetAngleAboutXRadians();
	return Vec3(x, lengthYZ * cosf(angleAboutXRadians + deltaRadians), lengthYZ * sinf(angleAboutXRadians + deltaRadians));
}

Vec3 const Vec3::GetRotatedAboutXDegrees(float deltaDegrees) const
{
	float lengthYZ = GetLengthYZ();
	float angleAboutXDegrees = GetAngleAboutXDegrees();
	return Vec3(x, lengthYZ * CosDegrees(angleAboutXDegrees + deltaDegrees), lengthYZ * SinDegrees(angleAboutXDegrees + deltaDegrees));
}

Vec3 const Vec3::GetRotated90DegreesAboutZ() const
{
	return Vec3(-y, x, z);
}

Vec3 const Vec3::GetRotated90DegreesAboutY() const
{
	return Vec3(z, y, -x);
}

Vec3 const Vec3::GetRotatedMinus90DegreesAboutY() const
{
	return Vec3(-z, y, x);
}

Vec3 const Vec3::GetRotated90DegreesAboutX() const
{
	return Vec3(x, -z, y);
}

Vec3 const Vec3::GetRotatedMinus90DegreesAboutX() const
{
	return Vec3(x, z, -y);
}

void Vec3::RotateAroundZRadians(float deltaRadians)
{
	float lengthXY = GetLengthXY();
	float angleAboutZRadians = GetAngleAboutZRadians();
	x = lengthXY * cosf(angleAboutZRadians + deltaRadians);
	y = lengthXY * sinf(angleAboutZRadians + deltaRadians);
}

void Vec3::RotateAroundZDegrees(float deltaDegrees)
{
	float lengthXY = GetLengthXY();
	float angleAboutZDegrees = GetAngleAboutZDegrees();
	x = lengthXY * CosDegrees(angleAboutZDegrees + deltaDegrees);
	y = lengthXY * SinDegrees(angleAboutZDegrees + deltaDegrees);
}

void Vec3::RotateAroundYRadians(float deltaRadians)
{
	float lengthZX = GetLengthZX();
	float angleAboutYRadians = GetAngleAboutYRadians();
	z = lengthZX * cosf(angleAboutYRadians + deltaRadians);
	x = lengthZX * sinf(angleAboutYRadians + deltaRadians);
}

void Vec3::RotateAroundYDegrees(float deltaDegrees)
{
	float lengthZX = GetLengthZX();
	float angleAboutYDegrees = GetAngleAboutYDegrees();
	z = lengthZX * CosDegrees(angleAboutYDegrees + deltaDegrees);
	x = lengthZX * SinDegrees(angleAboutYDegrees + deltaDegrees);
}

void Vec3::RotateAroundXRadians(float deltaRadians)
{
	float lengthYZ = GetLengthYZ();
	float angleAboutXRadians = GetAngleAboutXRadians();
	y = lengthYZ * cosf(angleAboutXRadians + deltaRadians);
	z = lengthYZ * sinf(angleAboutXRadians + deltaRadians);
}

void Vec3::RotateAroundXDegrees(float deltaDegrees)
{
	float lengthYZ = GetLengthYZ();
	float angleAboutXDegrees = GetAngleAboutXDegrees();
	y = lengthYZ * CosDegrees(angleAboutXDegrees + deltaDegrees);
	z = lengthYZ * SinDegrees(angleAboutXDegrees + deltaDegrees);
}

void Vec3::Normalize()
{
	float length = GetLength();
	if (length > 0.f)
	{
		float scalingFactor = 1.f / length;
		x *= scalingFactor;
		y *= scalingFactor;
	}
}

Vec3 const Vec3::GetClamped(float maxLength) const
{
	if (GetLengthSquared() > (maxLength * maxLength))
	{
		return GetNormalized() * maxLength;
	}

	return Vec3(x, y, z);
}

Vec3 const Vec3::GetNormalized() const
{
	float length = GetLength();
	if (length > 0)
	{
		return Vec3(x / length, y / length, z / length);
	}

	return Vec3();
}

Vec2 const Vec3::GetXY() const
{
	return Vec2(x, y);
}

Vec2 const Vec3::GetYZ() const
{
	return Vec2(y, z);
}

Vec2 const Vec3::GetZX() const
{
	return Vec2(z, x);
}
