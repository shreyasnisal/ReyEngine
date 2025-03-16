#pragma once

struct Mat44;
struct Vec3;

/*! Stores the EulerAngles as yaw, pitch and roll (in degrees)
* 
* EulerAngles represent rotations about the three axis independently. Since the order in which rotations are performed makes a difference to where the resultant vector points, the order is always as follows: rotate the graph paper about the upward axis first, then around the left axis, and finally about the forward axis. This is most commonly how FPS aiming feels and although other orders simply generate different effects, it could be very confusing in scenarios like FPS games.
* 
*/
class EulerAngles
{
public:
	//! Default destructor for EulerAngles objects
	~EulerAngles() = default;
	//! Default constructor for EulerAngles objects
	EulerAngles() = default;
	explicit EulerAngles(float yaw, float pitch, float roll);

	void SetFromText(char const* text);

	void GetAsVectors_iFwd_jLeft_kUp(Vec3& out_iBasis, Vec3& out_jBasis, Vec3& out_kBasis) const;
	Mat44 GetAsMatrix_iFwd_jLeft_kUp() const;

	EulerAngles const operator+(EulerAngles const& eulerAnglesToAdd) const;

public:
	//! The rotation about the upward axis
	float m_yawDegrees = 0.f;
	//! The rotation about the left axis
	float m_pitchDegrees = 0.f;
	//! The rotation about the forward axis
	float m_rollDegrees = 0.f;

public:
	//! Convenience name for EulerAngles(0.f, 0.f, 0.f)
	static const EulerAngles ZERO;
};

inline const EulerAngles EulerAngles::ZERO = EulerAngles(0.f, 0.f, 0.f);
