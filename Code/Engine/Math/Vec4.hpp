#pragma once

struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	~Vec4() = default;
	Vec4() = default;
	Vec4(Vec4 const& copyVec4) = default;
	explicit Vec4(float initialX, float initialY, float initialZ, float initialW);

	Vec4 const			operator+(Vec4 const& vecToAdd) const;
	void				operator+=(Vec4 const& vecToAdd);
	Vec4 const			operator-(Vec4 const& vecToSubtract) const;
	void				operator-=(Vec4 const& vecToSubtract);
	Vec4 const			operator*(float uniformScale) const;
	void				operator*=(float uniformScale);

	friend Vec4 const operator*(float uniformScale, Vec4 const& vecToScale);

	float				GetLength();

public:
	static const Vec4 ZERO;
};

inline const Vec4 Vec4::ZERO{0.f, 0.f, 0.f, 0.f};
