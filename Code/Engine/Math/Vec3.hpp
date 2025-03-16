#pragma once

struct Vec2;

struct Vec3
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

public:
	~Vec3() {}
	Vec3() {}
	Vec3(const Vec3& copyFrom);
	explicit Vec3(float initialX, float initialY, float initialZ);

	static Vec3 const	MakeFromPolarRadians(float latitudeRadians, float longitudeRadians, float length = 1.f);
	static Vec3 const	MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length = 1.f);
	static Vec3 const	MakeFromCylindricalRadians(float radialDistance, float azimuthRadians, float z);
	static Vec3 const	MakeFromCylindricalDegrees(float radialDistance, float azimuthDegrees, float z);

	void				SetFromText(char const* text);

	float				GetLength() const;
	float				GetLengthXY() const;
	float				GetLengthYZ() const;
	float				GetLengthZX() const;
	float				GetLengthSquared() const;
	float				GetLengthXYSquared() const;
	float				GetLengthYZSquared() const;
	float				GetLengthZXSquared() const;

	float				GetAngleAboutZRadians() const;
	float				GetAngleAboutZDegrees() const;
	float				GetAngleAboutYRadians() const;
	float				GetAngleAboutYDegrees() const;
	float				GetAngleAboutXRadians() const;
	float				GetAngleAboutXDegrees() const;

	Vec3 const			GetRotatedAboutZRadians(float deltaRadians) const;
	Vec3 const			GetRotatedAboutZDegrees(float deltaDegrees) const;
	Vec3 const			GetRotatedAboutYRadians(float deltaRadians) const;
	Vec3 const			GetRotatedAboutYDegrees(float deltaDegrees) const;
	Vec3 const			GetRotatedAboutXRadians(float deltaRadians) const;
	Vec3 const			GetRotatedAboutXDegrees(float deltaDegrees) const;
	Vec3 const			GetRotated90DegreesAboutZ() const;
	Vec3 const			GetRotated90DegreesAboutY() const;
	Vec3 const			GetRotatedMinus90DegreesAboutY() const;
	Vec3 const			GetRotated90DegreesAboutX() const;
	Vec3 const			GetRotatedMinus90DegreesAboutX() const;

	void				RotateAroundZRadians(float deltaRadians);
	void				RotateAroundZDegrees(float deltaDegrees);
	void				RotateAroundYRadians(float deltaRadians);
	void				RotateAroundYDegrees(float deltaDegrees);
	void				RotateAroundXRadians(float deltaRadians);
	void				RotateAroundXDegrees(float deltaDegrees);
	void				Normalize();

	Vec3 const			GetClamped(float maxLength) const;
	Vec3 const			GetNormalized() const;
	Vec2 const			GetXY() const;
	Vec2 const			GetYZ() const;
	Vec2 const			GetZX() const;

	bool				operator==(const Vec3& compare) const;
	bool				operator!=(const Vec3& compare) const;
	const Vec3			operator+(const Vec3& vecToAdd) const;
	const Vec3			operator-(const Vec3& vecToSubtract) const;
	const Vec3			operator-() const;
	const Vec3			operator*(float uniformScale) const;
	const Vec3			operator*(const Vec3& vecToMultiply) const;
	const Vec3			operator/(float inverseScale) const;

	void			operator+=(const Vec3& vecToAdd);
	void			operator-=(const Vec3& vecToSubtract);
	void			operator*=(const Vec3& vecToMultiply);
	void			operator*=(const float uniformScale);
	void			operator/=(const float uniformDivisor);
	void			operator=(const Vec3& copyFrom);


	friend const Vec3 operator*(float uniformScale, const Vec3& vecToScale);

public:
	static const Vec3	NORTH;
	static const Vec3	SOUTH;
	static const Vec3	EAST;
	static const Vec3	WEST;
	static const Vec3	SKYWARD;
	static const Vec3	GROUNDWARD;
	static const Vec3	ZERO;
};

inline const Vec3	Vec3::NORTH = Vec3(0.f, 1.f, 0.f);
inline const Vec3	Vec3::SOUTH = Vec3(0.f, -1.f, 0.f);
inline const Vec3	Vec3::EAST = Vec3(1.f, 0.f, 0.f);
inline const Vec3	Vec3::WEST = Vec3(-1.f, 0.f, 0.f);
inline const Vec3	Vec3::SKYWARD = Vec3(0.f, 0.f, 1.f);
inline const Vec3	Vec3::GROUNDWARD = Vec3(0.f, 0.f, -1.f);
inline const Vec3	Vec3::ZERO = Vec3(0.f, 0.f, 0.f);
