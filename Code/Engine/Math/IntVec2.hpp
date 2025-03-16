#pragma once

struct Vec2;

/*! \brief A two-dimensional vector for integer values
* 
* \sa Vec2
*/
struct IntVec2
{
public:
	//! The x coordinate of the IntVec2
	int x = 0;
	//! The y coordinate of the IntVec2
	int y = 0;

public:
	//! Default destructor for IntVec2 objects
	~IntVec2() = default;
	//! Default constructor for IntVec2 objects
	IntVec2() = default;
	//! Default copy constructor for IntVec2 objects
	IntVec2(IntVec2 const& copyFrom) = default;
	explicit IntVec2(int initialX, int initialY);

	void SetFromText(char const* text);

	float			GetLength() const;
	int				GetTaxicabLength() const;
	int				GetLengthSquared() const;
	float			GetOrientationRadians() const;
	float			GetOrientationDegrees() const;
	IntVec2 const	GetRotated90Degrees() const;
	IntVec2 const	GetRotatedMinus90Degrees() const;

	IntVec2 const	operator+(const IntVec2& intVecToAdd) const;
	IntVec2 const	operator-(const IntVec2& intVecToSubtract) const;
	IntVec2 const	operator-() const;
	IntVec2 const	operator*(int uniformScale) const;
	IntVec2 const	operator/(int inverseScale) const;
	bool			operator==(IntVec2 const& intVecToCompare) const;
	bool			operator!=(IntVec2 const& intVecToCompare) const;

	void			Rotate90Degrees();
	void			RotateMinus90Degrees();

	void			operator=(const IntVec2& copyFrom);
	void			operator+=(const IntVec2& intVecToAdd);
	void			operator-=(const IntVec2& intVecToSubtract);
	void			operator*=(int uniformScale);
	void			operator/=(int inverseScale);

	Vec2 const		GetAsVec2() const;


	friend IntVec2 const operator*(int uniformScale, IntVec2 const& intVecToScale);

	//! Convenience name for IntVec2(0, 1)
	static const IntVec2 NORTH;
	//! Convenience name for IntVec2(0, -1)
	static const IntVec2 SOUTH;
	//! Convenience name for IntVec2(1, 0)
	static const IntVec2 EAST;
	//! Convenience name for IntVec2(-1, 0)
	static const IntVec2 WEST;
	//! Convenience name for IntVec2(0, 0)
	static const IntVec2 ZERO;
	//! Convenience name for IntVec2(1, 1)
	static const IntVec2 ONE;
};

inline const IntVec2 IntVec2::NORTH{0, 1};
inline const IntVec2 IntVec2::SOUTH{0, -1};
inline const IntVec2 IntVec2::EAST{1, 0};
inline const IntVec2 IntVec2::WEST{-1, 0};
inline const IntVec2 IntVec2::ZERO{0, 0};
inline const IntVec2 IntVec2::ONE{1, 1};
