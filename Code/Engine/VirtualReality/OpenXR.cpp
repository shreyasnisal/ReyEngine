#include "Engine/VirtualReality/OpenXR.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SurfaceData.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#define XR_USE_PLATFORM_WIN32
#define XR_USE_GRAPHICS_API_D3D11
#include "ThirdParty/openxr/openxr_platform.h"
#include "ThirdParty/openxr/openxr_platform_defines.h"

#if defined(_DEBUG)
	#pragma comment(lib, "Thirdparty/openxr/openxr_loaderd.lib")
#else
	#pragma comment(lib, "Thirdparty/openxr/openxr_loader.lib")
#endif

PFN_xrGetD3D11GraphicsRequirementsKHR s_extXRGetD3D11GraphicsRequirementsKHR = nullptr;
PFN_xrCreateDebugUtilsMessengerEXT    s_extXRCreateDebugUtilsMessengerEXT = nullptr;
PFN_xrDestroyDebugUtilsMessengerEXT   s_extXRDestroyDebugUtilsMessengerEXT = nullptr;
PFN_xrCreateHandTrackerEXT g_xrCreateHandTrackerEXT = nullptr;
PFN_xrDestroyHandTrackerEXT g_xrDestroyHandTrackerEXT = nullptr;
PFN_xrLocateHandJointsEXT g_xrLocateHandJointsEXT = nullptr;


struct OpenXRSwapchain
{
public:
	XrSwapchain m_handle;
	int m_width = 0;
	int m_height = 0;
	std::vector<XrSwapchainImageD3D11KHR> m_surfaceImages;
	std::vector<SurfaceData> m_surfaceData;
};


OpenXR::OpenXR(OpenXRConfig config)
	: m_config (config)
{
}

void OpenXR::Startup()
{
	if (!g_openXR)
	{
		return;
	}

	std::vector<char const*> extensionsToUse;
	char const* extensionsToAskFor[] = { XR_KHR_D3D11_ENABLE_EXTENSION_NAME, XR_EXT_DEBUG_UTILS_EXTENSION_NAME, XR_EXT_HAND_TRACKING_EXTENSION_NAME };

	unsigned int extensionCount = 0;
	xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);
	std::vector<XrExtensionProperties> xrExtensions(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
	xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, xrExtensions.data());

	for (size_t extensionIndex = 0; extensionIndex < xrExtensions.size(); extensionIndex++)
	{
		for (int askExtensionIndex = 0; askExtensionIndex < _countof(extensionsToAskFor); askExtensionIndex++)
		{
			if (!strcmp(extensionsToAskFor[askExtensionIndex], xrExtensions[extensionIndex].extensionName))
			{
				extensionsToUse.push_back(extensionsToAskFor[askExtensionIndex]);
				break;
			}
		}

		if (!strcmp(xrExtensions[extensionIndex].extensionName, XR_EXT_HAND_TRACKING_EXTENSION_NAME))
		{
			m_isHandTrackingSupported = true;
		}
	}

	XrInstanceCreateInfo createInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
	createInfo.enabledExtensionCount = (unsigned int)extensionsToUse.size();
	createInfo.enabledExtensionNames = extensionsToUse.data();
	createInfo.applicationInfo.apiVersion = XR_API_VERSION_1_0;
	strcpy_s(createInfo.applicationInfo.applicationName, "VRSandbox1");
	XrResult xrResult = xrCreateInstance(&createInfo, &m_xrInstance);

	if (xrResult != XR_SUCCESS)
	{
		g_console->AddLine(DevConsole::WARNING, "Could not create XR instance! Aborting OpenXR initialization.");
		return;
	}

	xrGetInstanceProcAddr(m_xrInstance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)(&s_extXRCreateDebugUtilsMessengerEXT));
	xrGetInstanceProcAddr(m_xrInstance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction*)(&s_extXRDestroyDebugUtilsMessengerEXT));
	xrGetInstanceProcAddr(m_xrInstance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction*)(&s_extXRGetD3D11GraphicsRequirementsKHR));
	xrGetInstanceProcAddr(m_xrInstance, "xrCreateHandTrackerEXT", (PFN_xrVoidFunction*)&g_xrCreateHandTrackerEXT);
	xrGetInstanceProcAddr(m_xrInstance, "xrDestroyHandTrackerEXT", (PFN_xrVoidFunction*)&g_xrDestroyHandTrackerEXT);
	xrGetInstanceProcAddr(m_xrInstance, "xrLocateHandJointsEXT", (PFN_xrVoidFunction*)&g_xrLocateHandJointsEXT);

	XrDebugUtilsMessengerCreateInfoEXT debugInfo = { XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };

	debugInfo.messageTypes =
		XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;
	debugInfo.messageSeverities =
		XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	debugInfo.userCallback = DebugInfoUserCallback;
	if (s_extXRCreateDebugUtilsMessengerEXT)
	{
		s_extXRCreateDebugUtilsMessengerEXT(m_xrInstance, &debugInfo, &m_xrDebug);
	}

	XrSystemGetInfo systemInfo = { XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = m_appConfigForm;
	xrResult = xrGetSystem(m_xrInstance, &systemInfo, &m_xrSystemId);

	uint32_t blend_count = 0;
	xrResult = xrEnumerateEnvironmentBlendModes(m_xrInstance, m_xrSystemId, m_appConfigView, 1, &blend_count, &m_xrBlendMode);

	XrGraphicsRequirementsD3D11KHR requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
	s_extXRGetD3D11GraphicsRequirementsKHR(m_xrInstance, m_xrSystemId, &requirement);

	XrGraphicsBindingD3D11KHR binding = { XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
	binding.device = m_config.m_renderer->GetDevice();
	XrSessionCreateInfo sessionInfo = { XR_TYPE_SESSION_CREATE_INFO };
	sessionInfo.next = &binding;
	sessionInfo.systemId = m_xrSystemId;
	xrResult = xrCreateSession(m_xrInstance, &sessionInfo, &m_xrSession);

	if (xrResult != XR_SUCCESS)
	{
		g_console->AddLine(DevConsole::WARNING, "Could not create XR instance! Aborting OpenXR initialization.");
		//DebugAddMessage("Could not start XR session! Aborting OpenXR initialization", 10.f, Rgba8::RED, Rgba8(255, 0, 0, 0));
		return;
	}

	XrReferenceSpaceCreateInfo refSpace = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
	refSpace.poseInReferenceSpace = m_xrPoseIdentity;
	refSpace.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
	xrCreateReferenceSpace(m_xrSession, &refSpace, &m_xrAppSpace);

	unsigned int viewCount = 0;
	xrEnumerateViewConfigurationViews(m_xrInstance, m_xrSystemId, m_appConfigView, 0, &viewCount, nullptr);
	m_xrConfigViews.resize(viewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
	m_xrViews.resize(viewCount, { XR_TYPE_VIEW });
	xrEnumerateViewConfigurationViews(m_xrInstance, m_xrSystemId, m_appConfigView, viewCount, &viewCount, m_xrConfigViews.data());
	for (unsigned int viewIndex = 0; viewIndex < viewCount; viewIndex++)
	{
		XrViewConfigurationView& view = m_xrConfigViews[viewIndex];
		XrSwapchainCreateInfo swapchainInfo = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		XrSwapchain handle;
		swapchainInfo.arraySize = 1;
		swapchainInfo.mipCount = 1;
		swapchainInfo.faceCount = 1;
		swapchainInfo.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainInfo.width = view.recommendedImageRectWidth;
		swapchainInfo.height = view.recommendedImageRectHeight;
		swapchainInfo.sampleCount = view.recommendedSwapchainSampleCount;
		swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
		xrCreateSwapchain(m_xrSession, &swapchainInfo, &handle);

		unsigned int surfaceCount = 0;
		xrEnumerateSwapchainImages(handle, 0, &surfaceCount, nullptr);

		OpenXRSwapchain swapchain = {};
		swapchain.m_handle = handle;
		swapchain.m_width = swapchainInfo.width;
		swapchain.m_height = swapchainInfo.height;
		swapchain.m_surfaceImages.resize(surfaceCount, { XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR });
		swapchain.m_surfaceData.resize(surfaceCount);
		xrEnumerateSwapchainImages(swapchain.m_handle, surfaceCount, &surfaceCount, (XrSwapchainImageBaseHeader*)swapchain.m_surfaceImages.data());

		for (unsigned int surfaceIndex = 0; surfaceIndex < surfaceCount; surfaceIndex++)
		{
			swapchain.m_surfaceData[surfaceIndex] = m_config.m_renderer->CreateSurface(swapchain.m_width, swapchain.m_height, swapchain.m_surfaceImages[surfaceIndex].texture, true);
		}
		m_xrSwapchains.push_back(swapchain);
	}

	if (InitializeInput())
	{
		m_isInitialized = true;
	}
}

void OpenXR::BeginFrame()
{
	if (!m_isInitialized)
	{
		return;
	}

	UpdateSessionState();

	xrWaitFrame(m_xrSession, nullptr, &m_frameState);
	xrBeginFrame(m_xrSession, nullptr);

	for (int viewIndex = 0; viewIndex < NUM_VIEWS; ++viewIndex)
	{
		XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
		xrAcquireSwapchainImage(m_xrSwapchains[viewIndex].m_handle, &acquireInfo, &m_currentImageIndex[viewIndex]);

		XrSwapchainImageWaitInfo waitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
		waitInfo.timeout = XR_INFINITE_DURATION;
		xrWaitSwapchainImage(m_xrSwapchains[viewIndex].m_handle, &waitInfo);
	}

	unsigned int viewCount = 0;
	XrViewState viewState = { XR_TYPE_VIEW_STATE };
	XrViewLocateInfo locateInfo = { XR_TYPE_VIEW_LOCATE_INFO };
	locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	locateInfo.displayTime = m_frameState.predictedDisplayTime;
	locateInfo.space = m_xrAppSpace;
	xrLocateViews(m_xrSession, &locateInfo, &viewState, (unsigned int)m_xrViews.size(), &viewCount, m_xrViews.data());
	
	// Sync actions- Get actions info from controller
	XrResult result;
	XrActiveActionSet activeActionSets[2];
	activeActionSets[0].actionSet = m_leftController.GetActionSet();
	activeActionSets[0].subactionPath = m_leftController.GetHandPath();
	activeActionSets[1].actionSet = m_rightController.GetActionSet();
	activeActionSets[1].subactionPath = m_rightController.GetHandPath();
	XrActionsSyncInfo syncInfo = { XR_TYPE_ACTIONS_SYNC_INFO };
	syncInfo.countActiveActionSets = 2;
	syncInfo.activeActionSets = activeActionSets;
	result = xrSyncActions(m_xrSession, &syncInfo);

	m_leftController.Update();
	m_rightController.Update();

	if (m_isHandTrackingSupported)
	{
		m_leftHand.Update();
		m_rightHand.Update();
	}
}

void OpenXR::EndFrame()
{
	if (!m_isInitialized)
	{
		return;
	}

	for (int viewIndex = 0; viewIndex < NUM_VIEWS; viewIndex++)
	{
		XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
		xrReleaseSwapchainImage(m_xrSwapchains[viewIndex].m_handle, &releaseInfo);
	}

	XrCompositionLayerBaseHeader* layers = nullptr;
	XrCompositionLayerProjection layer = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
	layer.space = m_xrAppSpace;

	std::vector<XrCompositionLayerProjectionView> projectionViews(NUM_VIEWS);

	for (int viewIndex = 0; viewIndex < NUM_VIEWS; ++viewIndex)
	{
		XrCompositionLayerProjectionView& projView = projectionViews[viewIndex];
		projView = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
		projView.pose = m_xrViews[viewIndex].pose;
		projView.fov = m_xrViews[viewIndex].fov;
		projView.subImage.swapchain = m_xrSwapchains[viewIndex].m_handle;
		projView.subImage.imageRect.offset = { 0, 0 };
		projView.subImage.imageRect.extent = { m_xrSwapchains[viewIndex].m_width, m_xrSwapchains[viewIndex].m_height };
		projView.subImage.imageArrayIndex = 0;
	}

	layer.viewCount = (unsigned int)projectionViews.size();
	layer.views = projectionViews.data();

	layers = (XrCompositionLayerBaseHeader*)(&layer);

	XrFrameEndInfo endInfo = { XR_TYPE_FRAME_END_INFO };
	endInfo.displayTime = m_frameState.predictedDisplayTime;
	endInfo.environmentBlendMode = m_xrBlendMode;
	endInfo.layerCount = layers == nullptr ? 0 : 1;
	endInfo.layers = &layers;
	xrEndFrame(m_xrSession, &endInfo);
}

void OpenXR::Shutdown()
{
	if (!m_isInitialized)
	{
		return;
	}

	m_leftHand.Destroy();
	m_rightHand.Destroy();
	m_leftController.Destroy();
	m_rightController.Destroy();

	for (int swapchainIndex = 0; swapchainIndex < (int)m_xrSwapchains.size(); swapchainIndex++)
	{
		xrDestroySwapchain(m_xrSwapchains[swapchainIndex].m_handle);
		for (size_t surfaceIndex = 0; surfaceIndex < m_xrSwapchains[swapchainIndex].m_surfaceData.size(); surfaceIndex++)
		{
			DX_SAFE_RELEASE(m_xrSwapchains[swapchainIndex].m_surfaceData[surfaceIndex].m_backBufferView);
			DX_SAFE_RELEASE(m_xrSwapchains[swapchainIndex].m_surfaceData[surfaceIndex].m_depthStencilView);
		}
	}
	m_xrSwapchains.clear();

	if (m_xrAppSpace != XR_NULL_HANDLE)
	{
		xrDestroySpace(m_xrAppSpace);
	}
	if (m_xrSession != XR_NULL_HANDLE)
	{
		xrDestroySession(m_xrSession);
	}
	if (m_xrDebug != XR_NULL_HANDLE)
	{
		s_extXRDestroyDebugUtilsMessengerEXT(m_xrDebug);
	}
	if (m_xrInstance != XR_NULL_HANDLE)
	{
		xrDestroyInstance(m_xrInstance);
	}
}

ID3D11RenderTargetView* OpenXR::GetRenderTargetViewForEye(XREye eye) const
{
	if (!m_isInitialized)
	{
		return nullptr;
	}

	return m_xrSwapchains[(int)eye].m_surfaceData[m_currentImageIndex[(int)eye]].m_backBufferView;
}

ID3D11DepthStencilView* OpenXR::GetDepthStencilViewForEye(XREye eye) const
{
	if (!m_isInitialized)
	{
		return nullptr;
	}

	return m_xrSwapchains[(int)eye].m_surfaceData[m_currentImageIndex[(int)eye]].m_depthStencilView;
}

IntVec2 OpenXR::GetViewportDimensionsForEye(XREye eye) const
{
	if (!m_isInitialized)
	{
		return IntVec2::ZERO;
	}

	return IntVec2(m_xrSwapchains[(int)eye].m_width, m_xrSwapchains[(int)eye].m_height);
}

void OpenXR::GetFovsForEye(XREye eye, float& out_angleLeft, float& out_angleRight, float& out_angleUp, float& out_angleDown) const
{
	if (!m_isInitialized)
	{
		return;
	}

	XrFovf fovs = m_xrViews[(int)eye].fov;
	out_angleLeft = fovs.angleLeft;
	out_angleRight = fovs.angleRight;
	out_angleUp = fovs.angleUp;
	out_angleDown = fovs.angleDown;
}

void OpenXR::GetTransformForEye_iFwd_jLeft_kUp(XREye eye, Vec3& out_position, EulerAngles& out_orientation) const
{
	if (!m_isInitialized)
	{
		return;
	}

	XrPosef pose = m_xrViews[(int)eye].pose;
	// Swizzled values to support game (Squirrel) coordinate system
	out_position = Vec3(-pose.position.z, -pose.position.x, pose.position.y);
	out_orientation = GetEulerAnglesFromQuaternion(-pose.orientation.z, -pose.orientation.x, pose.orientation.y, pose.orientation.w);
}

XrBool32 OpenXR::DebugInfoUserCallback(XrDebugUtilsMessageSeverityFlagsEXT severity, XrDebugUtilsMessageTypeFlagsEXT types, const XrDebugUtilsMessengerCallbackDataEXT* msg, void* user_data)
{
	UNUSED(severity);
	UNUSED(types);
	UNUSED(user_data);

	DebuggerPrintf("[OpenXR]: %s: %s\n", msg->functionName, msg->message);

	return (XrBool32)XR_FALSE;
}

bool OpenXR::InitializeInput()
{
	m_leftController.Initialize();
	m_rightController.Initialize();

	XrPath oculusControllerPath;
	XrResult result = xrStringToPath(m_xrInstance, VRController::OCULUS_CONTROLLER_PATH_STR.c_str(), &oculusControllerPath);
	if (result != XR_SUCCESS)
	{
		g_console->AddLine(DevConsole::WARNING, "Could not create oculus controller path! Aborting OpenXR initialization...");
		//DebugAddMessage("Error in VR input initialization: Could not create oculus controller path! Aborting OpenXR initialization.", 10.f, Rgba8::RED, Rgba8(255, 0, 0, 0));
		return false;
	}

	// Create action bindings and suggest bindings to instance
	std::vector<XrActionSuggestedBinding> bindings;
	m_leftController.CreateActionBindings(bindings);
	m_rightController.CreateActionBindings(bindings);
	XrInteractionProfileSuggestedBinding suggestedBindings = { XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
	suggestedBindings.interactionProfile = oculusControllerPath;
	suggestedBindings.suggestedBindings = bindings.data();
	suggestedBindings.countSuggestedBindings = VRController::NUM_ACTION_BINDINGS * 2 + 1; // +1 for the Menu button only on the left controller- the corresponding button on the right controller is reserved for Quest menu
	result = xrSuggestInteractionProfileBindings(m_xrInstance, &suggestedBindings);
	if (result != XR_SUCCESS)
	{
		g_console->AddLine(DevConsole::WARNING, "Could not suggest interaction profile binding! Aborting OpenXR initialization...");
		//DebugAddMessage("Error in VR input initialization: Could not suggest interaction profile binding! Aborting OpenXR initialization.", 10.f, Rgba8::RED, Rgba8(255, 0, 0, 0));
		return false;
	}

	// Attach both action sets to the session
	XrActionSet actionSets[] = { m_leftController.GetActionSet(), m_rightController.GetActionSet() };

	XrSessionActionSetsAttachInfo attachInfo = { XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
	attachInfo.countActionSets = 2;
	attachInfo.actionSets = actionSets;
	result = xrAttachSessionActionSets(m_xrSession, &attachInfo);

	if (result != XR_SUCCESS)
	{
		g_console->AddLine(DevConsole::WARNING, "Could not attach action set to session! Aborting OpenXR initialization...");
		//DebugAddMessage("Error in VR input visualization: Could not attach action set to session! Aborting OpenXR initialization.", 10.f, Rgba8::RED, Rgba8(255, 0, 0, 0));
		return false;
	}

	// Hand Tracking
	XrSystemHandTrackingPropertiesEXT handTrackingProperties = { XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT };
	XrSystemProperties systemProperties = { XR_TYPE_SYSTEM_PROPERTIES, &handTrackingProperties };
	xrGetSystemProperties(m_xrInstance, m_xrSystemId, &systemProperties);
	m_isHandTrackingSupported = (bool)handTrackingProperties.supportsHandTracking;
	if (!m_isHandTrackingSupported)
	{
		g_console->AddLine(DevConsole::WARNING, "Hand tracking not supported. Initializing OpenXR without hand tracking.");
		//DebugAddMessage(Stringf("Hand tracking not supported! Initializing OpenXR without hand tracking."), 10.f, Rgba8::RED, Rgba8(255, 0, 0, 0));
		return true;
	}

	m_leftHand.Initialize();
	m_rightHand.Initialize();

	return true;
}

void OpenXR::UpdateSessionState()
{
	if (!m_isInitialized)
	{
		return;
	}

	XrEventDataBuffer eventBuffer = { XR_TYPE_EVENT_DATA_BUFFER };

	while (xrPollEvent(m_xrInstance, &eventBuffer) == XR_SUCCESS)
	{
		switch (eventBuffer.type)
		{
			case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
			{
				XrEventDataSessionStateChanged* changed = (XrEventDataSessionStateChanged*)&eventBuffer;
				m_xrSessionState = changed->state;

				switch (m_xrSessionState)
				{
					case XR_SESSION_STATE_READY:
					{
						XrSessionBeginInfo beginInfo = { XR_TYPE_SESSION_BEGIN_INFO };
						beginInfo.primaryViewConfigurationType = m_appConfigView;
						xrBeginSession(m_xrSession, &beginInfo);
						break;
					}
					case XR_SESSION_STATE_STOPPING:
					{
						xrEndSession(m_xrSession);
						//FireEvent("Quit");
						break;
					}
					case XR_SESSION_STATE_EXITING:
					{
						xrEndSession(m_xrSession);
						FireEvent("Quit");
						break;
					}
				}
			}
		}
		eventBuffer = { XR_TYPE_EVENT_DATA_BUFFER };
	}
}

