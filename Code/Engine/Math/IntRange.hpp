#pragma once

/*! \brief Represents an integer range from a min integer value to a max integer value
* 
*/
struct IntRange
{
public:
	//! The lower bound for the integer range
	int m_min = 0;
	//! The upper bound for the integer range
	int m_max = 0;

public:
	//! Default destructor for IntRange objects
	~IntRange() = default;
	//! Default constructor for IntRange objects
	IntRange() = default;
	//! Default copy constructor for IntRange objects
	IntRange(IntRange const& copyFrom) = default;
	explicit IntRange(int min, int max);

	void operator=(IntRange const& intRangeToAssign);
	bool operator==(IntRange const& intRangeToCompare) const;
	bool operator!=(IntRange const& intRangeToCompare) const;

	bool IsOnRange(int value) const;
	bool IsOverlappingWith(IntRange const& otherIntRange) const;
	void SetFromText(char const* text);

public:
	//! Convenience name for IntRange(0, 0)
	static const IntRange ZERO;
	//! Convenience name for IntRange(1, 1)
	static const IntRange ONE;
	//! Convenience name for IntRange(0, 1)
	static const IntRange ZERO_TO_ONE;
};

inline const IntRange IntRange::ZERO{0, 0};
inline const IntRange IntRange::ONE{1, 1};
inline const IntRange IntRange::ZERO_TO_ONE{0, 1};
