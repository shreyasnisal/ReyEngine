#include "Engine/VirtualReality/VRHand.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/VirtualReality/OpenXR.hpp"


VRHand::~VRHand()
{
}

VRHand::VRHand(XRHand hand)
	: m_hand(hand)
{
}

void VRHand::Initialize()
{
	if (m_hand == XRHand::NONE)
	{
		ERROR_AND_DIE("Attempted to intiialize a hand that wasn't set to left or right!");
	}

	XrSession xrSession = g_openXR->GetSession();

	XrHandTrackerCreateInfoEXT handCreateInfo = { XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT };
	handCreateInfo.handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT;
	handCreateInfo.hand = (m_hand == XRHand::LEFT ? XR_HAND_LEFT_EXT : XR_HAND_RIGHT_EXT);
	g_xrCreateHandTrackerEXT(xrSession, &handCreateInfo, &m_tracker);

	m_initialized = true;
}

void VRHand::Update()
{
	XrSpace appSpace = g_openXR->GetAppSpace();
	XrTime predictedDisplayTime = g_openXR->GetPredictedFrameTime();

	XrHandJointLocationEXT handJointLocations[XR_HAND_JOINT_COUNT_EXT];
	XrHandJointLocationsEXT handJointLocationsInfo = { XR_TYPE_HAND_JOINT_LOCATIONS_EXT };
	handJointLocationsInfo.jointCount = XR_HAND_JOINT_COUNT_EXT;
	handJointLocationsInfo.jointLocations = handJointLocations;
	XrHandJointsLocateInfoEXT handJointsLocateInfo = { XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT };
	handJointsLocateInfo.baseSpace = appSpace;
	handJointsLocateInfo.time = predictedDisplayTime;
	g_xrLocateHandJointsEXT(m_tracker, &handJointsLocateInfo, &handJointLocationsInfo);

	for (int jointIndex = 0; jointIndex < XR_HAND_JOINT_COUNT_EXT; jointIndex++)
	{
		XrHandJointLocationEXT const& jointLocation = handJointLocations[jointIndex];

		if (jointLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT)
		{
			XrPosef jointPose = jointLocation.pose;
			m_joints[jointIndex].m_position = Vec3(-jointPose.position.z, -jointPose.position.x, jointPose.position.y);
			m_joints[jointIndex].m_orientation = GetEulerAnglesFromQuaternion(-jointPose.orientation.z, jointPose.orientation.x, jointPose.orientation.y, jointPose.orientation.w);
			m_joints[jointIndex].m_radius = jointLocation.radius;
		}
	}
}

void VRHand::Destroy()
{
	if (g_xrDestroyHandTrackerEXT && m_tracker != XR_NULL_HANDLE)
	{
		g_xrDestroyHandTrackerEXT(m_tracker);
	}
}

void VRHand::GetAllJoints_iFwd_jLeft_kUp(std::vector<VRHandJoint>& out_joints) const
{
	for (int jointIndex = 0; jointIndex < XR_HAND_JOINT_COUNT_EXT; jointIndex++)
	{
		out_joints.push_back(m_joints[jointIndex]);
	}
}
