#pragma once

#pragma once

struct Vec3;

/*! \brief A three-dimensional vector for integer values
*
* \sa Vec3
*/
struct IntVec3
{
public:
	//! The x coordinate of the IntVec3
	int x = 0;
	//! The y coordinate of the IntVec3
	int y = 0;
	//! The z-coordinate of the IntVec3
	int z = 0;

public:
	//! Default destructor for IntVec3 objects
	~IntVec3() = default;
	//! Default constructor for IntVec3 objects
	IntVec3() = default;
	//! Default copy constructor for IntVec3 objects
	IntVec3(IntVec3 const& copyFrom) = default;
	explicit IntVec3(int initialX, int initialY, int initialZ);

	void SetFromText(char const* text);

	float			GetLength() const;
	int				GetTaxicabLength() const;
	int				GetLengthSquared() const;

	IntVec3 const	operator+(const IntVec3& intVecToAdd) const;
	IntVec3 const	operator-(const IntVec3& intVecToSubtract) const;
	IntVec3 const	operator-() const;
	IntVec3 const	operator*(int uniformScale) const;
	IntVec3 const	operator/(int inverseScale) const;
	bool			operator==(IntVec3 const& intVecToCompare) const;
	bool			operator!=(IntVec3 const& intVecToCompare) const;

	void			operator=(const IntVec3& copyFrom);
	void			operator+=(const IntVec3& intVecToAdd);
	void			operator-=(const IntVec3& intVecToSubtract);
	void			operator*=(int uniformScale);
	void			operator/=(int inverseScale);

	Vec3 const		GetAsVec3() const;


	friend IntVec3 const operator*(int uniformScale, IntVec3 const& intVecToScale);

	//! Convenience name for IntVec3(0, 1)
	static const IntVec3 NORTH;
	//! Convenience name for IntVec3(0, -1)
	static const IntVec3 SOUTH;
	//! Convenience name for IntVec3(1, 0)
	static const IntVec3 EAST;
	//! Convenience name for IntVec3(-1, 0)
	static const IntVec3 WEST;
	//! Convenience name for IntVec3(0, 0, -1)
	static const IntVec3 GROUNDWARD;
	//! Convenience name for IntVec3(0, 0, 1)
	static const IntVec3 SKYWARD;
	//! Convenience name for IntVec3(0, 0)
	static const IntVec3 ZERO;
	//! Convenience name for IntVec3(1, 1)
	static const IntVec3 ONE;
};

inline const IntVec3 IntVec3::NORTH{ 0, 1, 0 };
inline const IntVec3 IntVec3::SOUTH{ 0, -1, 0 };
inline const IntVec3 IntVec3::EAST{ 1, 0, 0 };
inline const IntVec3 IntVec3::WEST{ -1, 0, 0 };
inline const IntVec3 IntVec3::GROUNDWARD{ 0, 0, -1 };
inline const IntVec3 IntVec3::SKYWARD{ 0, 0, 1 };
inline const IntVec3 IntVec3::ZERO{ 0, 0, 0 };
inline const IntVec3 IntVec3::ONE{ 1, 1, 1 };

