#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"

#include "ThirdParty/openxr/openxr.h"

struct VRHandJoint
{
public:
	Vec3 m_position = Vec3::ZERO;
	EulerAngles m_orientation = EulerAngles::ZERO;
	float m_radius = 0.f;
};

class VRHand
{
	friend class OpenXR;

public:
	~VRHand();
	VRHand(XRHand hand);
	void Initialize();
	void Update();
	void Destroy();

	int GetNumJoints() { return XR_HAND_JOINT_COUNT_EXT; }
	void GetAllJoints_iFwd_jLeft_kUp(std::vector<VRHandJoint>& out_joints) const;

private:
	bool m_initialized = false;
	XRHand m_hand = XRHand::NONE;
	XrHandTrackerEXT m_tracker = nullptr;
	VRHandJoint m_joints[XR_HAND_JOINT_COUNT_EXT];
};
