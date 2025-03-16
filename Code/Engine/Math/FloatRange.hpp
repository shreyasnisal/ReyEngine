#pragma once

/*! \brief Represents a float range from a min float value to a max float value
* 
*/
struct FloatRange
{
public:
	//! Lower bound for the float range
	float m_min = 0.f;
	//! Upper bound for the float range
	float m_max = 0.f;

public:
	//! Default destructor for FloatRange objects
	~FloatRange() = default;
	//! Default constructor for FloatRange objects
	FloatRange() = default;
	//! Default copy constructor for FloatRange objects
	FloatRange(FloatRange const& copyFrom) = default;
	explicit FloatRange(float min, float max);

	void operator=(FloatRange const& floatRangeToAssign);
	bool operator==(FloatRange const& floatRangeToCompare) const;
	bool operator!=(FloatRange const& floatRangeToCompare) const;

	bool IsOnRange(float value) const;
	bool IsOverlappingWith(FloatRange const& otherFloatRange) const;
	void SetFromText(char const* text);

public:
	//! Convenience name for FloatRange(0.f, 0.f)
	static const FloatRange ZERO;
	//! Convenience name for FloatRange(1.f, 1.f)
	static const FloatRange ONE;
	//! Convenience name for FloatRange(0.f, 1.f)
	static const FloatRange ZERO_TO_ONE;
};

inline const FloatRange FloatRange::ZERO{0.f, 0.f};
inline const FloatRange FloatRange::ONE{1.f, 1.f};
inline const FloatRange FloatRange::ZERO_TO_ONE{0.f, 1.f};