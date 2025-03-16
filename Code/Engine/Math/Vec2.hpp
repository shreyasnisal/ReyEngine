#pragma once

struct Vec3;

struct Vec2
{
public:
	float x = 0.f;
	float y = 0.f;

public:
	~Vec2() = default;
	Vec2() = default;
	Vec2( Vec2 const& copyFrom );
	explicit Vec2( float initialX, float initialY );

	static Vec2 const MakeFromPolarRadians(float orientationRadians, float length = 1.f);
	static Vec2 const MakeFromPolarDegrees(float orientationDegrees, float length = 1.f);

	void SetFromText(char const* text);

	float			GetLength() const;
	float			GetLengthSquared() const;
	float			GetOrientationDegrees() const;
	float			GetOrientationRadians() const;
	Vec2 const		GetRotated90Degrees() const;
	Vec2 const		GetRotatedMinus90Degrees() const;
	Vec2 const		GetRotatedRadians(float deltaRadians) const;
	Vec2 const		GetRotatedDegrees(float deltaDegrees) const;
	Vec2 const		GetClamped(float maxLength) const;
	Vec2 const		GetNormalized() const;
	Vec2 const		GetReflected(Vec2 const& surfaceNormal) const;

	void			SetOrientationRadians(float newOrientationRadians);
	void			SetOrientationDegrees(float newOrientationDegrees);
	void			SetPolarRadians(float newOrientationRadians, float newLength);
	void			SetPolarDegrees(float newOrientationDegrees, float newLength);
	void			Rotate90Degrees();
	void			RotateMinus90Degrees();
	void			RotateRadians(float deltaRadians);
	void			RotateDegrees(float deltaDegrees);
	void			SetLength(float newLength);
	void			ClampLength(float maxLength);
	void			Normalize();
	float			NormalizeAndGetPreviousLength();
	void			Reflect(Vec2 const& surfaceNormal);

	bool			operator==(Vec2 const& compare) const;
	bool			operator!=(Vec2 const& compare) const;
	Vec2 const		operator+(Vec2 const& vecToAdd) const;
	Vec2 const		operator-(Vec2 const& vecToSubtract) const;
	Vec2 const		operator-() const;
	Vec2 const		operator*(float uniformScale) const;
	Vec2 const		operator*(Vec2 const& vecToMultiply) const;
	Vec2 const		operator/(float inverseScale) const;

	void			operator+=(Vec2 const& vecToAdd);
	void			operator-=(Vec2 const& vecToSubtract);
	void			operator*=(float uniformScale);
	void			operator/=(float uniformDivisor);
	void			operator=(Vec2 const& copyFrom);

	Vec3 const		ToVec3(float z = 0.f) const;


	friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );

public:
	static const Vec2 EAST;
	static const Vec2 WEST;
	static const Vec2 NORTH;
	static const Vec2 SOUTH;
	static const Vec2 ZERO;
	static const Vec2 ONE;
};


inline const Vec2 Vec2::EAST{1.f, 0.f};
inline const Vec2 Vec2::WEST{-1.f, 0.f};
inline const Vec2 Vec2::NORTH{0.f, 1.f};
inline const Vec2 Vec2::SOUTH{0.f, -1.f};
inline const Vec2 Vec2::ZERO{0.f, 0.f};
inline const Vec2 Vec2::ONE{1.f, 1.f};
