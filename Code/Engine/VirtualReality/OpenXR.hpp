#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/VirtualReality/VRController.hpp"
#include "Engine/VirtualReality/VRHand.hpp"

#include "ThirdParty/openxr/openxr.h"

#include <vector>


class Renderer;
struct OpenXRSwapchain;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
class VRController;


extern PFN_xrCreateHandTrackerEXT g_xrCreateHandTrackerEXT;
extern PFN_xrDestroyHandTrackerEXT g_xrDestroyHandTrackerEXT;
extern PFN_xrLocateHandJointsEXT g_xrLocateHandJointsEXT;

struct OpenXRConfig
{
public:
	Renderer* m_renderer = nullptr;
};

class OpenXR
{
public:
	~OpenXR() = default;
	OpenXR(OpenXRConfig config);

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	ID3D11RenderTargetView* GetRenderTargetViewForEye(XREye eye) const;
	ID3D11DepthStencilView* GetDepthStencilViewForEye(XREye eye) const;
	IntVec2 GetViewportDimensionsForEye(XREye eye) const;
	void GetFovsForEye(XREye eye, float& out_angleLeft, float& out_angleRight, float& out_angleUp, float& out_angleDown) const;
	void GetTransformForEye_iFwd_jLeft_kUp(XREye eye, Vec3& out_position, EulerAngles& out_orientation) const;

	XrSession GetSession() const { return m_xrSession; }
	XrInstance GetInstance() const{ return m_xrInstance; }
	VRController& GetLeftController() { return m_leftController; }
	VRController const& GetLeftController() const { return m_leftController; }
	VRController& GetRightController() { return m_rightController; }
	VRController const& GetRightController() const { return m_rightController; }
	VRHand const& GetLeftHand() const { return m_leftHand; }
	VRHand const& GetRightHand() const { return m_rightHand; }
	
	XrTime GetPredictedFrameTime() const { return m_frameState.predictedDisplayTime; }
	XrSpace GetAppSpace() const { return m_xrAppSpace; }

	bool IsInitialized() const { return m_isInitialized; }

	static XrBool32 DebugInfoUserCallback(XrDebugUtilsMessageSeverityFlagsEXT severity, XrDebugUtilsMessageTypeFlagsEXT types, const XrDebugUtilsMessengerCallbackDataEXT* msg, void* user_data);

protected:
	bool InitializeInput();
	void UpdateSessionState();

protected:
	OpenXRConfig m_config;

	bool m_isInitialized = false;

	XrPosef m_xrPoseIdentity = { {0,0,0,1}, {0,0,0} };
	XrInstance m_xrInstance = {};
	XrSession m_xrSession = {};
	XrSessionState m_xrSessionState = XR_SESSION_STATE_UNKNOWN;
	XrSpace m_xrAppSpace = {};
	XrSystemId m_xrSystemId = XR_NULL_SYSTEM_ID;
	XrEnvironmentBlendMode m_xrBlendMode = {};
	XrDebugUtilsMessengerEXT m_xrDebug = {};
	XrFormFactor m_appConfigForm = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	XrViewConfigurationType m_appConfigView = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	XrFrameState m_frameState = { XR_TYPE_FRAME_STATE };

	std::vector<XrView> m_xrViews;
	std::vector<XrViewConfigurationView> m_xrConfigViews;
	std::vector<OpenXRSwapchain> m_xrSwapchains;

	static constexpr unsigned int NUM_VIEWS = 2;
	unsigned int m_currentImageIndex[2] = {0, 0};

	VRController m_leftController = VRController(XRHand::LEFT);
	VRController m_rightController = VRController(XRHand::RIGHT);

	VRHand m_leftHand = VRHand(XRHand::LEFT);
	VRHand m_rightHand = VRHand(XRHand::RIGHT);

	bool m_isHandTrackingSupported = false;
};
