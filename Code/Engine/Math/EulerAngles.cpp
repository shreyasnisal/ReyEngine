#include "Engine/Math/EulerAngles.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"

/*! \brief Constructs an EulerAngles object from a given yaw, pitch and roll
* 
* \param yaw The yaw value in degrees
* \param pitch The pitch value in degrees
* \param roll The roll value in degrees
* 
*/
EulerAngles::EulerAngles(float yaw, float pitch, float roll)
	: m_yawDegrees(yaw)
	, m_pitchDegrees(pitch)
	, m_rollDegrees(roll)
{
}

/*! \brief Constructs the EulerAngles from a string
*
* Fatal error if the number of comma-separated values in the string are not 3 or 4. For 3 comma-separated values, an alpha of 255 is used. If the number of comma-separated components is correct but any of the components cannot be parsed to an integer, that component is set to a value of 0.
* \param text The string to be parsed for the color
*
*/
void EulerAngles::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, ',');

	if (numSplitStrings != 3)
	{
		ERROR_AND_DIE("Incorrect number of literals in EulerAngles string!");
	}

	m_yawDegrees = static_cast<float>(atof(splitStrings[0].c_str()));
	m_pitchDegrees = static_cast<float>(atof(splitStrings[1].c_str()));
	m_rollDegrees = static_cast<float>(atof(splitStrings[2].c_str()));
}

/*! \brief Converts the EulerAngles to the forward, left and up vectors
* 
* The forward, left and up vectors are returned through the arguments passed in by reference, and are normalized.
* \param out_iBasis A Vec3 to hold the forward vector
* \param out_jBasis A Vec3 to hold the left vector
* \param out_kBasis A Vec3 to hold the up vector
* 
*/
void EulerAngles::GetAsVectors_iFwd_jLeft_kUp(Vec3& out_iBasis, Vec3& out_jBasis, Vec3& out_kBasis) const
{
	Mat44 eulerAnglesMatrix = GetAsMatrix_iFwd_jLeft_kUp();
	out_iBasis = eulerAnglesMatrix.GetIBasis3D();
	out_jBasis = eulerAnglesMatrix.GetJBasis3D();
	out_kBasis = eulerAnglesMatrix.GetKBasis3D();
}

/*! \brief Creates a matrix based on the EulerAngles
* 
* \return A Mat44 that transforms points based on the rotations specified by the EulerAngles
* 
*/
Mat44 EulerAngles::GetAsMatrix_iFwd_jLeft_kUp() const
{
	Mat44 matrix = Mat44();
	matrix.AppendZRotation(m_yawDegrees);
	matrix.AppendYRotation(m_pitchDegrees);
	matrix.AppendXRotation(m_rollDegrees);
	return matrix;
}

EulerAngles const EulerAngles::operator+(EulerAngles const& eulerAnglesToAdd) const
{
	return EulerAngles(m_yawDegrees + eulerAnglesToAdd.m_yawDegrees, m_pitchDegrees + eulerAnglesToAdd.m_pitchDegrees, m_rollDegrees + eulerAnglesToAdd.m_rollDegrees);
}
