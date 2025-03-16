#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Models/Model.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/VirtualReality/OpenXR.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

#include "ThirdParty/stb/stb_image.h"
#include <string>

#if defined(OPAQUE)
	#undef OPAQUE
#endif

struct CameraConstants
{
	Mat44 viewMatrix;
	Mat44 projectionMatrix;
};

struct ModelConstants
{
	Mat44 modelMatrix;
	float modelColor[4];
};

static LightConstants s_lightConstants;

struct BlurSample
{
public:
	Vec2 Offset;
	float Weight;
	float padding;
};

static constexpr int k_blurMaxSamples = 64;

struct BlurConstants
{
public:
	Vec2 TexelSize;
	float LerpT;
	int NumSamples;
	BlurSample Samples[k_blurMaxSamples];
};

static const int k_lightConstantsSlot = 1;
static const int k_cameraConstantsSlot = 2;
static const int k_modelConstantsSlot = 3;
static const int k_blurConstantsSlot = 5;

Renderer::Renderer(RenderConfig config)
	: m_config(config)
{
	
}

void Renderer::Startup()
{
	unsigned int deviceFlags = 0;
	#if defined(ENGINE_DEBUG_RENDER)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	// Create debug module
	#if defined(ENGINE_DEBUG_RENDER)
		m_dxgiDebugModule = reinterpret_cast<void*>(::LoadLibraryA("dxgidebug.dll"));
		if (m_dxgiDebugModule == nullptr)
		{
			ERROR_AND_DIE("Could not load dxgidebug.dll!");
		}

		typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
		((GetDebugModuleCB)::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))
			(__uuidof(IDXGIDebug), &m_dxgiDebug);

		if (m_dxgiDebug == nullptr)
		{
			ERROR_AND_DIE("Could not load debug module!");
		}
	#endif

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferDesc.Width = m_config.m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = m_config.m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = reinterpret_cast<HWND>(m_config.m_window->GetHwnd());
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create D3D11 device and swap chain!");
	}

	ID3D11Texture2D* backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not get swap chain buffer!");
	}

	SurfaceData backBufferSurface = CreateSurface(m_config.m_window->GetClientDimensions().x, m_config.m_window->GetClientDimensions().y, backBuffer);
	m_backBufferRTV = backBufferSurface.m_backBufferView;
	m_depthStencilView = backBufferSurface.m_depthStencilView;
	
	DX_SAFE_RELEASE(backBuffer);

	m_defaultShader = CreateShader("Default", g_defaultShader);
	BindShader(m_defaultShader);

	m_immediateVBO = CreateVertexBuffer(sizeof(Vertex_PCU));

	Vertex_PCU fullscreenVertexes[] = {
		Vertex_PCU(Vec3(-1.f, -1.f, 0.5f), Rgba8::WHITE, Vec2(0.f, 1.f)),
		Vertex_PCU(Vec3(1.f, -1.f, 0.5f), Rgba8::WHITE, Vec2(1.f, 1.f)),
		Vertex_PCU(Vec3(1.f, 1.f, 0.5f), Rgba8::WHITE, Vec2(1.f, 0.f)),

		Vertex_PCU(Vec3(-1.f, -1.f, 0.5f), Rgba8::WHITE, Vec2(0.f, 1.f)),
		Vertex_PCU(Vec3(1.f, 1.f, 0.5f), Rgba8::WHITE, Vec2(1.f, 0.f)),
		Vertex_PCU(Vec3(-1.f, 1.f, 0.5f), Rgba8::WHITE, Vec2(0.f, 0.f))
	};

	m_fullscreenVBO = CreateVertexBuffer(sizeof(fullscreenVertexes));
	CopyCPUToGPU(fullscreenVertexes, sizeof(fullscreenVertexes), m_fullscreenVBO);

	CreateRasterizerStates();

	// Create camera constants constant buffer
	m_cameraCBO = CreateConstantBuffer(sizeof(CameraConstants));

	// Create Blend States
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int) (BlendMode::OPAQUE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed!");
	}

	blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ALPHA)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed!");
	}

	blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ADDITIVE)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed!");
	}

	// Create Sampler Modes
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)(SamplerMode::POINT_CLAMP)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::POINT_CLAMP failed!");
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)(SamplerMode::BILINEAR_WRAP)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::BILINEAR_WRAP failed!");
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)(SamplerMode::BILINEAR_CLAMP)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::BILINEAR_WRAP failed!");
	}


	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_depthSamplerComparisonState);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for DepthSamplerComparisonState!");
	}

	m_deviceContext->PSSetSamplers(1, 1, &m_depthSamplerComparisonState);

	// Create default 1x1 texture
	Image defaultTextureImage = Image(IntVec2(1, 1), Rgba8::WHITE);
	m_defaultTexture = CreateTextureFromImage("Default", defaultTextureImage);
	BindTexture(m_defaultTexture);

	m_modelCBO = CreateConstantBuffer(sizeof(ModelConstants));
	SetModelConstants();

	m_lightCBO = CreateConstantBuffer(sizeof(LightConstants));

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)(DepthMode::ENABLED)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth enabled stencil state!");
	}

	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)(DepthMode::DISABLED)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth disabled stencil state!");
	}

	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)(DepthMode::READ_ONLY_LESS_EQUAL)]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth disabled stencil state!");
	}

	m_emissiveRTV = CreateRenderTargetTexture("EmissiveRTV", m_config.m_window->GetClientDimensions());
	m_blurredEmissiveRTV = CreateRenderTargetTexture("BlurredEmissiveRTV", m_config.m_window->GetClientDimensions());
	m_blurCBO = CreateConstantBuffer(sizeof(BlurConstants));

	int textureIndex = 1;
	int textureHeight = m_config.m_window->GetClientDimensions().y / 2;
	while (textureHeight >= 64)
	{
		// Create texture
		IntVec2 textureDimensions(int((float)textureHeight * m_config.m_window->GetAspect()), textureHeight);
		Texture* blurDownTexture = CreateRenderTargetTexture(Stringf("BlurDownTexture%d", textureIndex).c_str(), textureDimensions);
		
		// Add texture and texture dimensions for viewport setting to a list
		m_blurDownRTVs.push_back(blurDownTexture);

		// Do twice for blurdown and blurup and use blurup list in reverse order
		// Don't create absolute smallest blurup texture
		Texture* blurUpTexture = CreateRenderTargetTexture(Stringf("BlurUpTexture%d", textureIndex).c_str(), textureDimensions);
		m_blurUpRTVs.push_back(blurUpTexture);

		textureIndex++;
		textureHeight = textureHeight / 2;
	}

	IntVec2 textureDimensions(int((float)textureHeight * m_config.m_window->GetAspect()), textureHeight);
	Texture* blurDownTexture = CreateRenderTargetTexture(Stringf("BlurDownTexture%d", textureIndex).c_str(), textureDimensions);
	m_blurDownRTVs.push_back(blurDownTexture);

	hr = m_deviceContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), reinterpret_cast<void**>(&m_userDefinedAnnotations));
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create user defined annotations interface!");
	}

}

void Renderer::CreateRasterizerStates()
{
	// Create rasterizer states
	D3D11_RASTERIZER_DESC rasterizerDesc = { 0 };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerFillMode::SOLID)  + ((int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::CULL_NONE))]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create solid no-cull rasterizer state!");
	}

	rasterizerDesc = { 0 };
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerFillMode::WIREFRAME)  + ((int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::CULL_NONE))]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create wireframe no-cull rasterizer state!");
	}

	rasterizerDesc = { 0 };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerFillMode::SOLID)  + ((int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::CULL_FRONT))]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create solid front-cull rasterizer state!");
	}

	rasterizerDesc = { 0 };
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerFillMode::WIREFRAME)  + ((int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::CULL_FRONT))]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create wireframe front-cull rasterizer state!");
	}

	rasterizerDesc = { 0 };
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerFillMode::SOLID)  + ((int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::CULL_BACK))]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create solid back-cull rasterizer state!");
	}

	rasterizerDesc = { 0 };
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[(int)(RasterizerFillMode::WIREFRAME)  + ((int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::CULL_BACK))]);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create wireframe back-cull rasterizer state!");
	}
}

SurfaceData Renderer::CreateSurface(int width, int height, ID3D11Texture2D* renderTargetView, bool isOpenXRTexture)
{
	SurfaceData createdSurface = {};
	HRESULT hr;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc = {};
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetDesc.Format = (DXGI_FORMAT)DXGI_FORMAT_R8G8B8A8_UNORM;
	hr = m_device->CreateRenderTargetView(renderTargetView, isOpenXRTexture ? &targetDesc : nullptr, &createdSurface.m_backBufferView);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create render target view!");
	}

	ID3D11Texture2D* depthTexture;
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = width;
	depthDesc.Height = height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.Format = isOpenXRTexture ? DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.BindFlags = (isOpenXRTexture ? D3D11_BIND_SHADER_RESOURCE : 0 ) | D3D11_BIND_DEPTH_STENCIL;
	depthDesc.SampleDesc.Count = 1;
	hr = m_device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth texture!");
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	hr = m_device->CreateDepthStencilView(depthTexture, isOpenXRTexture ? &depthStencilDesc : nullptr, &createdSurface.m_depthStencilView);

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth stencil view!");
	}

	DX_SAFE_RELEASE(depthTexture);

	return createdSurface;
}

void Renderer::BeginFrame()
{
#if defined (USE_OPENXR)
	// Do nothing: If using OpenXR, game code must call BeginRederForEye to set the appropriate render target and depth stencil
#else
	BeginRenderForEye();
#endif
}

void Renderer::EndFrame()
{
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will now terminate!");
	}
}

void Renderer::Shutdown()
{
	// Clear cached shaders
	for (int shaderIndex = 0; shaderIndex < static_cast<int>(m_loadedShaders.size()); shaderIndex++)
	{
		delete m_loadedShaders[shaderIndex];
		m_loadedShaders[shaderIndex] = nullptr;
	}
	m_loadedShaders.clear();

	// Clear cached textures
	for (int textureIndex = 0; textureIndex < static_cast<int>(m_loadedTextures.size()); textureIndex++)
	{
		delete m_loadedTextures[textureIndex];
		m_loadedTextures[textureIndex] = nullptr;
	}

	// Clear cached bitmap fonts
	for (int bitmapFontIndex = 0; bitmapFontIndex < static_cast<int>(m_loadedFonts.size()); bitmapFontIndex++)
	{
		delete m_loadedFonts[bitmapFontIndex];
		m_loadedFonts[bitmapFontIndex] = nullptr;
	}

	// Release blend states
	for (int blendStateIndex = 0; blendStateIndex < (int) (BlendMode::COUNT); blendStateIndex++)
	{
		DX_SAFE_RELEASE(m_blendStates[blendStateIndex]);
	}

	// Release Sampler states
	for (int samplerStateIndex = 0; samplerStateIndex < (int)(SamplerMode::COUNT); samplerStateIndex++)
	{
		DX_SAFE_RELEASE(m_samplerStates[samplerStateIndex]);
	}
	DX_SAFE_RELEASE(m_depthSamplerComparisonState);

	// Release all rasterizer states
	for (int rasterizerStateIndex = 0; rasterizerStateIndex < ((int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::COUNT)); rasterizerStateIndex++)
	{
		DX_SAFE_RELEASE(m_rasterizerStates[rasterizerStateIndex]);
	}

	// Release all depth stencils
	for (int depthStencilStateIndex = 0; depthStencilStateIndex < (int)(DepthMode::COUNT); depthStencilStateIndex++)
	{
		DX_SAFE_RELEASE(m_depthStencilStates[depthStencilStateIndex]);
	}

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_fullscreenVBO;
	m_fullscreenVBO = nullptr;

	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	delete m_modelCBO;
	m_modelCBO = nullptr;

	delete m_lightCBO;
	m_lightCBO = nullptr;

	delete m_blurCBO;
	m_blurCBO = nullptr;

	DX_SAFE_RELEASE(m_backBufferRTV);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_depthStencilView);
	DX_SAFE_RELEASE(m_depthStencilTexture);
	DX_SAFE_RELEASE(m_userDefinedAnnotations);

	// Report error leaks and release debug module
	#if defined(ENGINE_DEBUG_RENDER)
		((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
			DXGI_DEBUG_ALL,
			(DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
		);

		IDXGIDebug* debugModule = (IDXGIDebug*)(m_dxgiDebug);
		DX_SAFE_RELEASE(debugModule);
		m_dxgiDebug = nullptr;

		::FreeLibrary((HMODULE)m_dxgiDebugModule);
		m_dxgiDebugModule = nullptr;
	#endif
}

void Renderer::BeginRenderForEye(XREye eye)
{
	m_currentEye = eye;

	if (eye == XREye::NONE || !g_openXR || !g_openXR->IsInitialized())
	{
		ID3D11RenderTargetView* rtvs[] = {m_backBufferRTV, m_emissiveRTV->m_renderTargetView};
		m_deviceContext->OMSetRenderTargets(2, rtvs, m_depthStencilView);
		return;
	}

	ID3D11RenderTargetView* renderTargetView = g_openXR->GetRenderTargetViewForEye(eye);
	ID3D11DepthStencilView* depthStencilView = g_openXR->GetDepthStencilViewForEye(eye);

	m_deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void Renderer::ClearScreen(const Rgba8& clearColor)
{
	BeginRenderEvent("Clear Screen");

	if (m_currentEye == XREye::NONE || !g_openXR || !g_openXR->IsInitialized())
	{
		BeginRenderEvent("Swapchain RTV");
		ClearRTV(clearColor);
		EndRenderEvent("Swapchain RTV");

		BeginRenderEvent("Emissive RTV");
		ClearRTV(Rgba8::BLACK, m_emissiveRTV);
		EndRenderEvent("Emissive RTV");

		BeginRenderEvent("Blurred Emissive RTV");
		ClearRTV(Rgba8::BLACK, m_blurredEmissiveRTV);
		EndRenderEvent("Blurred Emissive RTV");

		BeginRenderEvent("Intermediate Emissive RTVs : BlurDown");
		for (int textureIndex = 0; textureIndex < (int)m_blurDownRTVs.size(); textureIndex++)
		{
			ClearRTV(Rgba8::BLACK, m_blurDownRTVs[textureIndex]);
		}
		EndRenderEvent("Intermediate Emissive RTVs : BlurDown");

		BeginRenderEvent("Intermediate emissive RTVs : BlurUp");
		for (int textureIndex = 0; textureIndex < (int)m_blurUpRTVs.size(); textureIndex++)
		{
			ClearRTV(Rgba8::BLACK, m_blurUpRTVs[textureIndex]);
		}
		EndRenderEvent("Intermediate Emissive RTVs : BlurUp");

		BeginRenderEvent("Swapchain DSV");
		ClearDSV();
		EndRenderEvent("Swapchain DSV");
	}
	else
	{
		BeginRenderEvent("XR RTV and DSV");
		ID3D11RenderTargetView* renderTargetView = g_openXR->GetRenderTargetViewForEye(m_currentEye);
		ID3D11DepthStencilView* depthStencilView = g_openXR->GetDepthStencilViewForEye(m_currentEye);
		
		float clearColorAsFloats[4];
		clearColor.GetAsFloats(clearColorAsFloats);
		m_deviceContext->ClearRenderTargetView(renderTargetView, clearColorAsFloats);
		m_deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		EndRenderEvent("XR RTV and DSV");
	}

	EndRenderEvent("Clear Screen");
}

void Renderer::ClearRTV(Rgba8 const& clearColor, Texture* texture)
{
	float clearColorAsFloats[4];
	clearColor.GetAsFloats(clearColorAsFloats);
	if (!texture)
	{
		m_deviceContext->ClearRenderTargetView(m_backBufferRTV, clearColorAsFloats);
		return;
	}
	BeginRenderEvent("Clear Custom RTV");
	m_deviceContext->ClearRenderTargetView(texture->m_renderTargetView, clearColorAsFloats);
	EndRenderEvent("Clear Custom RTV");
}

void Renderer::ClearDSV(Texture* texture)
{
	if (!texture)
	{
		m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		return;
	}
	if (!texture->m_depthStencilView)
	{
		ERROR_AND_DIE(Stringf("Attempted to clear depth stencil view on texture \"%s\" with uninitialized DSV!", texture->m_name.c_str()));
	}
	BeginRenderEvent("Clear Custom DSV");
	m_deviceContext->ClearDepthStencilView(texture->m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	EndRenderEvent("Clear Custom DSV");
}

void Renderer::BeginCamera(const Camera& camera)
{
	// Set viewport
	D3D11_VIEWPORT viewport = { 0 };

	if (m_currentEye == XREye::NONE || !g_openXR || !g_openXR->IsInitialized())
	{
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.Width = static_cast<float>(m_config.m_window->GetClientDimensions().x);
		viewport.Height = static_cast<float>(m_config.m_window->GetClientDimensions().y);

		if (camera.GetViewport().m_mins.x != -1.f)
		{
			viewport.TopLeftX = camera.GetViewport().m_mins.x;
			viewport.TopLeftY = camera.GetViewport().m_maxs.y;
			viewport.Width = camera.GetViewport().GetDimensions().x;
			viewport.Height = camera.GetViewport().GetDimensions().y;
		}
		else if (camera.GetNormalizedViewportDimensions().x != -1.f)
		{
			viewport.TopLeftX = camera.GetDirectXViewportTopLeft().x;
			viewport.TopLeftY = camera.GetDirectXViewportTopLeft().y;
			viewport.Width = camera.GetDirectXViewportWidth();
			viewport.Height = camera.GetDirectXViewportHeight();
		}
	}
	else
	{
		IntVec2 viewportDimensions = g_openXR->GetViewportDimensionsForEye(m_currentEye);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.Width = (float)viewportDimensions.x;
		viewport.Height = (float)viewportDimensions.y;
	}

	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	m_deviceContext->RSSetViewports(1, &viewport);

	CameraConstants cameraConstants;
	cameraConstants.projectionMatrix = camera.GetProjectionMatrix();
	cameraConstants.viewMatrix = camera.GetViewMatrix();
	CopyCPUToGPU(reinterpret_cast<void*>(&cameraConstants), sizeof(cameraConstants), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);
}

void Renderer::EndCamera(const Camera& camera)
{
	UNUSED(camera);
	// Set viewport
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = static_cast<float>(m_config.m_window->GetClientDimensions().x);
	viewport.Height = static_cast<float>(m_config.m_window->GetClientDimensions().y);
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	m_deviceContext->RSSetViewports(1, &viewport);

	CameraConstants cameraConstants;
	CopyCPUToGPU(reinterpret_cast<void*>(&cameraConstants), sizeof(cameraConstants), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);
}

void Renderer::SetRTV(Texture* texture)
{
	if (!texture)
	{
		m_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
		return;
	}
	m_deviceContext->OMSetRenderTargets(1, &texture->m_renderTargetView, nullptr);
}

void Renderer::SetDSV(Texture* texture)
{
	m_deviceContext->PSSetShader(NULL,NULL,0);
	m_deviceContext->OMSetRenderTargets(0, NULL, texture->m_depthStencilView);
}

void Renderer::BindDepthBuffer(Texture* texture)
{
	if (!texture)
	{
		m_deviceContext->PSSetShaderResources(1, 1, &m_defaultTexture->m_shaderResourceView);
		return;
	}
	m_deviceContext->PSSetShaderResources(1, 1, &texture->m_shaderResourceView);
}

void Renderer::DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes)
{
	m_immediateVBO->m_stride = sizeof(Vertex_PCU);
	CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_PCU), m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, numVertexes);
}

void Renderer::DrawVertexArray(std::vector<Vertex_PCU> const& vertexes)
{
	m_immediateVBO->m_stride = sizeof(Vertex_PCU);
	DrawVertexArray(static_cast<int>(vertexes.size()), vertexes.data());
}

void Renderer::DrawVertexArray(std::vector<Vertex_PCUTBN> const& vertexes)
{
	m_immediateVBO->m_stride = sizeof(Vertex_PCUTBN);
	CopyCPUToGPU(vertexes.data(), (int)vertexes.size() * sizeof(Vertex_PCUTBN), m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, (int)vertexes.size());
}

void Renderer::DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, int vertexOffset)
{
	SetStatesIfChanged();
	BindVertexBuffer(vbo);
	m_deviceContext->Draw(vertexCount, vertexOffset);
}

void Renderer::DrawIndexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, int indexCount)
{
	SetStatesIfChanged();
	BindVertexBuffer(vbo);
	BindIndexBuffer(ibo);
	m_deviceContext->DrawIndexed(indexCount, 0, 0);
}

Texture* Renderer::CreateOrGetTextureFromFile(char const* imageFilePath)
{
	Texture* existingTexture = GetTextureFromFileName(imageFilePath);
	if (existingTexture)
	{
		return existingTexture;
	}

	return CreateTextureFromFile(imageFilePath);
}

Texture* Renderer::GetTextureFromFileName(char const* name)
{
	for (int textureIndex = 0; textureIndex < static_cast<int>(m_loadedTextures.size()); textureIndex++)
	{
		if (!strcmp(m_loadedTextures[textureIndex]->m_name.c_str(), name))
		{
			return m_loadedTextures[textureIndex];
		}
	}

	return nullptr;
}

Texture* Renderer::CreateTextureFromFile(char const* imageFilePath)
{
	Image image = Image(imageFilePath);
	return CreateTextureFromImage(imageFilePath, image);
}

Texture* Renderer::CreateTextureFromImage(char const* name, Image const& image)
{
	Texture* newTexture = new Texture();
	newTexture->m_name = name;
	newTexture->m_dimensions = image.GetDimensions();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = image.GetDimensions().x;
	textureDesc.Height = image.GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = image.GetRawData();
	textureData.SysMemPitch = 4 * image.GetDimensions().x;

	HRESULT hr = m_device->CreateTexture2D(&textureDesc, &textureData, &newTexture->m_texture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateTextureFromImage failed for image file \"%s\"", image.GetImageFilePath().c_str()));
	}

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL, &newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for image file \"%s\"", image.GetImageFilePath().c_str()));
	}

	m_loadedTextures.push_back(newTexture);
	return newTexture;
}

Texture* Renderer::CreateRenderTargetTexture(char const* name, IntVec2 const& dimensions)
{
	Texture* newTexture = new Texture();
	newTexture->m_name = name;
	newTexture->m_dimensions = dimensions;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = dimensions.x;
	textureDesc.Height = dimensions.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	HRESULT hr = m_device->CreateTexture2D(&textureDesc, NULL, &newTexture->m_texture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateTextureFromImage failed for render target texture \"%s\"", name));
	}

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL, &newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for render target texture \"%s\"", name));
	}

	hr = m_device->CreateRenderTargetView(newTexture->m_texture, NULL, &newTexture->m_renderTargetView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateRenderTargetView failed for render target texture \"%s\"", name));
	}

	m_loadedTextures.push_back(newTexture);
	return newTexture;
}

Texture* Renderer::CreateDepthBuffer(char const* name, IntVec2 const& dimensions)
{
	Texture* newTexture = new Texture();
	newTexture->m_name = name;
	newTexture->m_dimensions = dimensions;

	HRESULT hr;
	D3D11_TEXTURE2D_DESC depthBufferTextureDesc = {};
	depthBufferTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferTextureDesc.MipLevels = 1;
	depthBufferTextureDesc.ArraySize = 1;
	depthBufferTextureDesc.SampleDesc.Count = 1;
	depthBufferTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthBufferTextureDesc.Height = dimensions.y;
	depthBufferTextureDesc.Width = dimensions.x;

	hr = m_device->CreateTexture2D(&depthBufferTextureDesc, NULL, &newTexture->m_texture);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateTexture2D failed for depth buffer \"%s\"", name));
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	
	hr = m_device->CreateDepthStencilView(newTexture->m_texture, &depthStencilViewDesc, &newTexture->m_depthStencilView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateDepthStencilView failed for depth buffer \"%s\"", name));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, &shaderResourceViewDesc, &newTexture->m_shaderResourceView);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for depth buffer \"%s\"", name));
	}

	m_loadedTextures.push_back(newTexture);
	return newTexture;
}

void Renderer::BindTexture(Texture const* texture, int textureSlot)
{
	if (!texture)
	{
		texture = m_defaultTexture;
	}

	m_deviceContext->PSSetShaderResources(textureSlot, 1, &texture->m_shaderResourceView);
}

void Renderer::SetBlendMode(BlendMode blendMode)
{
	m_desiredBlendMode = blendMode;
}

void Renderer::SetSamplerMode(SamplerMode samplerMode)
{
	m_desiredSamplerMode = samplerMode;
}

void Renderer::SetModelConstants(Mat44 const& modelMatrix, Rgba8 const& modelColor)
{
	ModelConstants modelConstants;
	modelConstants.modelMatrix = modelMatrix;
	modelColor.GetAsFloats(modelConstants.modelColor);
	CopyCPUToGPU(reinterpret_cast<void*>(&modelConstants), sizeof(modelConstants), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

void Renderer::SetLightConstants(Vec3 const& lightDirection, float lightIntensity, float ambientLightIntensity, Vec3 const& worldEyePosition)
{
	s_lightConstants.SunDirection = lightDirection;
	s_lightConstants.SunIntensity = lightIntensity;
	s_lightConstants.AmbientIntensity = ambientLightIntensity;

	Vec3 lightJBasis = CrossProduct3D(Vec3::SKYWARD, lightDirection).GetNormalized();
	Vec3 lightKBasis = CrossProduct3D(lightDirection, lightJBasis).GetNormalized();
	s_lightConstants.LightViewMatrix = Mat44(lightDirection, lightJBasis, lightKBasis, Vec3::ZERO).GetOrthonormalInverse();

	s_lightConstants.LightProjectionMatrix = Mat44::CreateOrthoProjection(-75.f, 15.f, -25.f, 25.f, -20.f, 50.f);
	s_lightConstants.LightProjectionMatrix.Append(Mat44(Vec3::SKYWARD, Vec3::WEST, Vec3::NORTH, Vec3::ZERO));

	s_lightConstants.WorldEyePosition = worldEyePosition;

	CopyCPUToGPU(reinterpret_cast<void*>(&s_lightConstants), sizeof(s_lightConstants), m_lightCBO);
	BindConstantBuffer(k_lightConstantsSlot, m_lightCBO);
}

void Renderer::SetLightConstants(LightConstants const& lightConstants)
{
	s_lightConstants = lightConstants;

	CopyCPUToGPU(reinterpret_cast<void*>(&s_lightConstants), sizeof(s_lightConstants), m_lightCBO);
	BindConstantBuffer(k_lightConstantsSlot, m_lightCBO);
}

void Renderer::SetRasterizerFillMode(RasterizerFillMode fillMode)
{
	m_desiredRasterizerFillMode = fillMode;
}

void Renderer::SetRasterizerCullMode(RasterizerCullMode cullMode)
{
	m_desiredRasterizerCullMode = cullMode;
}

void Renderer::SetDepthMode(DepthMode depthMode)
{
	m_desiredDepthStencilMode = depthMode;
}

void Renderer::SetStatesIfChanged()
{
	if (m_blendStates[(int)m_desiredBlendMode] != m_blendState)
	{
		m_blendState = m_blendStates[(int)(m_desiredBlendMode)];
		float blendFactor[] = { 0.f, 0.f, 0.f, 0.f };
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, sampleMask);
	}

	if (m_samplerStates[(int)m_desiredSamplerMode] != m_samplerState)
	{
		m_samplerState = m_samplerStates[(int)(m_desiredSamplerMode)];
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}

	if (m_rasterizerStates[(int)m_desiredRasterizerFillMode + ((int)(RasterizerFillMode::COUNT) * (int)m_desiredRasterizerCullMode)] != m_rasterizerState)
	{
		m_rasterizerState = m_rasterizerStates[(int)m_desiredRasterizerFillMode + ((int)(RasterizerFillMode::COUNT) * (int)m_desiredRasterizerCullMode)];
		m_deviceContext->RSSetState(m_rasterizerState);
	}

	if (m_depthStencilStates[(int)m_desiredDepthStencilMode] != m_depthStencilState)
	{
		m_depthStencilState = m_depthStencilStates[(int)m_desiredDepthStencilMode];
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	}
}

BitmapFont* Renderer::CreateOrGetBitmapFont(const char* bitmapFontFilePathWithNoExtension)
{
	BitmapFont* existingFont = GetBitmapFontFromFileName(bitmapFontFilePathWithNoExtension);
	if (existingFont)
	{
		return existingFont;
	}

	BitmapFont* newFont = CreateBitmapFromFile(bitmapFontFilePathWithNoExtension);
	return newFont;
}

BitmapFont* Renderer::GetBitmapFontFromFileName(char const* bitmapFontName)
{
	for (int fontIndex = 0; fontIndex < static_cast<int>(m_loadedFonts.size()); fontIndex++)
	{
		if (!strcmp(m_loadedFonts[fontIndex]->m_fontFilePathNameWithNoExtension.c_str(), bitmapFontName))
		{
			return m_loadedFonts[fontIndex];
		}
	}

	return nullptr;
}

BitmapFont* Renderer::CreateBitmapFromFile(char const* bitmapFontFilePathWithNoExtension)
{
	std::string bitmapFontName = bitmapFontFilePathWithNoExtension;
	bitmapFontName += ".png";

	Image image = Image(bitmapFontName.c_str());
	Texture* newFontTexture = CreateTextureFromImage(bitmapFontName.c_str(), image);

	BitmapFont* newFont = new BitmapFont(bitmapFontFilePathWithNoExtension, newFontTexture);

	m_loadedFonts.push_back(newFont);
	return newFont;
}

Shader* Renderer::CreateOrGetShader(char const* shaderName, VertexType vertexType)
{
	Shader* existingShader = GetShaderFromFileName(shaderName);
	if (existingShader)
	{
		return existingShader;
	}

	Shader* newShader = CreateShader(shaderName, vertexType);

	return newShader;
}

Shader* Renderer::GetShaderFromFileName(char const* shaderName)
{
	for (int shaderIndex = 0; shaderIndex < (int)m_loadedShaders.size(); shaderIndex++)
	{
		if (!strcmp(m_loadedShaders[shaderIndex]->m_config.m_name.c_str(), shaderName))
		{
			return m_loadedShaders[shaderIndex];
		}
	}

	return nullptr;
}

Shader* Renderer::CreateShader(char const* shaderName, VertexType vertexType)
{
	std::string shaderFilename = shaderName;
	shaderFilename += ".hlsl";
	std::string shaderSource;
	if (!FileReadToString(shaderSource, shaderFilename.c_str()))
	{
		ERROR_RECOVERABLE(Stringf("Could not read shader %s", shaderFilename.c_str()));
	}

	return CreateShader(shaderName, shaderSource.c_str(), vertexType);
}

Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource, VertexType vertexType)
{
	ShaderConfig shaderConfig;
	shaderConfig.m_name = shaderName;
	Shader* out_shader = new Shader(shaderConfig);

	HRESULT hr;

	// Create vertex shader
	std::vector<unsigned char> vertexShaderByteCode;
	if (!CompileShaderToByteCode(vertexShaderByteCode, "VertexShader", shaderSource, shaderConfig.m_vertexEntryPoint.c_str(), "vs_5_0"))
	{
		ERROR_AND_DIE(Stringf("Could not compile vertex shader code for shader \"%s\"!", shaderName));
	}
	
	hr = m_device->CreateVertexShader(
		vertexShaderByteCode.data(),
		vertexShaderByteCode.size(),
		NULL, &out_shader->m_vertexShader
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create vertex shader for shader \"%s\"!", shaderName));
	}


	// Create pixel shader
	std::vector<unsigned char> pixelShaderByteCode;
	if (!CompileShaderToByteCode(pixelShaderByteCode, "PixelShader", shaderSource, shaderConfig.m_pixelEntryPoint.c_str(), "ps_5_0"))
	{
		ERROR_AND_DIE(Stringf("Could not compile pixel shader code for shader \"%s\"!", shaderName));
	}

	hr = m_device->CreatePixelShader(
		pixelShaderByteCode.data(),
		pixelShaderByteCode.size(),
		NULL, &out_shader->m_pixelShader
	);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create pixel shader for shader \"%s\"!", shaderName));
	}

	// Create input layout and set on shader
	if (vertexType == VertexType::VERTEX_PCU)
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		UINT numElements = ARRAYSIZE(inputElementDesc);
		hr = m_device->CreateInputLayout(
			inputElementDesc, numElements,
			vertexShaderByteCode.data(),
			vertexShaderByteCode.size(),
			&out_shader->m_inputLayout
		);
	}
	else if (vertexType == VertexType::VERTEX_PCUTBN)
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		UINT numElements = ARRAYSIZE(inputElementDesc);
		hr = m_device->CreateInputLayout(
			inputElementDesc, numElements,
			vertexShaderByteCode.data(),
			vertexShaderByteCode.size(),
			&out_shader->m_inputLayout
		);
	}

	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex layout!");
	}

	m_loadedShaders.push_back(out_shader);
	return out_shader;
}

bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& out_byteCode, char const* name, char const* source, char const* entryPoint, char const* target)
{
	DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
	#if defined(ENGINE_DEBUG_RENDER)
		shaderFlags = D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	ID3DBlob* shaderBlob = NULL;
	ID3DBlob* errorBlob = NULL;

	HRESULT hr;

	hr = D3DCompile(
		source, strlen(source),
		name, nullptr, nullptr,
		entryPoint, target, shaderFlags, 0,
		&shaderBlob, &errorBlob
	);
	if (SUCCEEDED(hr))
	{
		out_byteCode.resize(shaderBlob->GetBufferSize());
		memcpy(
			out_byteCode.data(),
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize()
		);
		DX_SAFE_RELEASE(shaderBlob);

		return true;
	}
	else
	{
		if (errorBlob != NULL)
		{
			DebuggerPrintf((char*)errorBlob->GetBufferPointer());
			DX_SAFE_RELEASE(errorBlob);
		}
		DX_SAFE_RELEASE(shaderBlob);
	}

	return false;
}

void Renderer::BindShader(Shader* shader)
{
	if (!shader)
	{
		shader = m_defaultShader;
	}

	m_currentShader = shader;
	m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);

	m_deviceContext->IASetInputLayout(shader->m_inputLayout);
}

VertexBuffer* Renderer::CreateVertexBuffer(size_t const size, VertexType vertexType, bool isLinePrimitive) const
{
	VertexBuffer* vbo = new VertexBuffer(size);
	vbo->m_isLinePrimitive = isLinePrimitive;

	if (vertexType == VertexType::VERTEX_PCU)
	{
		vbo->m_stride = sizeof(Vertex_PCU);
	}
	else if (vertexType == VertexType::VERTEX_PCUTBN)
	{
		vbo->m_stride = sizeof(Vertex_PCUTBN);
	}

	UINT vertexBufferSize = static_cast<UINT>(size);
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = vertexBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &vbo->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create vertex buffer!");
	}

	return vbo;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, VertexBuffer*& vbo) const
{
	if (vbo->m_size < size)
	{
		int stride = vbo->m_stride;
		delete vbo;
		vbo = CreateVertexBuffer(size);
		vbo->m_stride = stride;
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(vbo->m_buffer, 0);
}

void Renderer::BindVertexBuffer(VertexBuffer* vbo)
{
	UINT stride = vbo->m_stride;
	UINT startOffset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &vbo->m_buffer, &stride, &startOffset);
	m_deviceContext->IASetPrimitiveTopology(vbo->m_isLinePrimitive ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

ConstantBuffer* Renderer::CreateConstantBuffer(size_t const size) const
{
	ConstantBuffer* cbo = new ConstantBuffer(size);

	UINT vertexBufferSize = static_cast<UINT>(size);
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = vertexBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &cbo->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create constant buffer!");
	}

	return cbo;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, ConstantBuffer* cbo) const
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(cbo->m_buffer, 0);
}

void Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

IndexBuffer* Renderer::CreateIndexBuffer(size_t const size) const
{
	IndexBuffer* ibo = new IndexBuffer(size);

	UINT indexBufferSize = static_cast<UINT>(size);
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = indexBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, &ibo->m_buffer);
	if (!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create index buffer!");
	}

	return ibo;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, IndexBuffer*& ibo) const
{
	if (ibo->m_size < size)
	{
		delete ibo;
		ibo = CreateIndexBuffer(size);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(ibo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, size);
	m_deviceContext->Unmap(ibo->m_buffer, 0);
}

void Renderer::BindIndexBuffer(IndexBuffer* ibo)
{
	m_deviceContext->IASetIndexBuffer(ibo->m_buffer, DXGI_FORMAT_R32_UINT, 0);
}

void Renderer::RenderEmissive()
{
	BeginRenderEvent("Emissive");

	BeginRenderEvent("Emissive Blur Down");
	//-------------------------------------------------------------------------------------------------------------------------------------------
	BlurConstants downSamplingBlurConstants = {};
	downSamplingBlurConstants.LerpT = 1.f;
	downSamplingBlurConstants.NumSamples = 13;
	downSamplingBlurConstants.TexelSize = Vec2(1.f / (float)m_config.m_window->GetClientDimensions().x, 1.f / (float)m_config.m_window->GetClientDimensions().y);
	downSamplingBlurConstants.Samples[0] = { Vec2(-2.f, -2.f), 0.0323f };
	downSamplingBlurConstants.Samples[1] = { Vec2(-2.f, 0.f), 0.0645f };
	downSamplingBlurConstants.Samples[2] = { Vec2(-2.f, 2.f), 0.0323f };
	downSamplingBlurConstants.Samples[3] = { Vec2(-1.f, -1.f), 0.129f };
	downSamplingBlurConstants.Samples[4] = { Vec2(-1.f, 1.f), 0.129f };
	downSamplingBlurConstants.Samples[5] = { Vec2(0.f, -2.f), 0.0645f };
	downSamplingBlurConstants.Samples[6] = { Vec2(0.f, 0.f), 0.0968f };
	downSamplingBlurConstants.Samples[7] = { Vec2(0.f, 2.f), 0.0645f };
	downSamplingBlurConstants.Samples[8] = { Vec2(1.f, -1.f), 0.129f };
	downSamplingBlurConstants.Samples[9] = { Vec2(1.f, 1.f), 0.129f };
	downSamplingBlurConstants.Samples[10] = { Vec2(2.f, -2.f), 0.0323f };
	downSamplingBlurConstants.Samples[11] = { Vec2(2.f, 0.f), 0.0645f };
	downSamplingBlurConstants.Samples[12] = { Vec2(2.f, 2.f), 0.0323f };

	CopyCPUToGPU((void*)&downSamplingBlurConstants, sizeof(downSamplingBlurConstants), m_blurCBO);
	BindConstantBuffer(k_blurConstantsSlot, m_blurCBO);

	Shader* blurDownShader = CreateOrGetShader("Data/Shaders/BlurDown");

	m_deviceContext->OMSetRenderTargets(1, &m_blurDownRTVs[0]->m_renderTargetView, nullptr);
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = (float)m_blurDownRTVs[0]->m_dimensions.x;
	viewport.Height = (float)m_blurDownRTVs[0]->m_dimensions.y;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_deviceContext->RSSetViewports(1, &viewport);
	SetDepthMode(DepthMode::DISABLED);
	SetBlendMode(BlendMode::OPAQUE);
	SetSamplerMode(SamplerMode::BILINEAR_CLAMP);
	SetRasterizerCullMode(RasterizerCullMode::CULL_BACK);
	SetRasterizerFillMode(RasterizerFillMode::SOLID);
	BindShader(blurDownShader);
	BindTexture(m_emissiveRTV);
	DrawVertexBuffer(m_fullscreenVBO, 6);

	for (int rtvIndex = 1; rtvIndex < (int)m_blurDownRTVs.size(); rtvIndex++)
	{
		downSamplingBlurConstants.TexelSize = Vec2(1.f / (float)m_blurDownRTVs[rtvIndex - 1]->m_dimensions.x, 1.f / (float)m_blurDownRTVs[rtvIndex - 1]->m_dimensions.y);
		CopyCPUToGPU((void*)&downSamplingBlurConstants, sizeof(downSamplingBlurConstants), m_blurCBO);
		BindConstantBuffer(k_blurConstantsSlot, m_blurCBO);

		SetRTV(m_blurDownRTVs[rtvIndex]);
		BindTexture(m_blurDownRTVs[rtvIndex - 1]);
		viewport = {};
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.Width = (float)m_blurDownRTVs[rtvIndex]->m_dimensions.x;
		viewport.Height = (float)m_blurDownRTVs[rtvIndex]->m_dimensions.y;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_deviceContext->RSSetViewports(1, &viewport);
		DrawVertexBuffer(m_fullscreenVBO, 6);
	}

	EndRenderEvent("Emissive Blur Down");


	BeginRenderEvent("Emissive Blur Up");
	//-------------------------------------------------------------------------------------------------------------------------------------------
	// Blur Up
	SetRTV(m_blurUpRTVs[m_blurUpRTVs.size() - 1]);
	Shader* blurUpShader = CreateOrGetShader("Data/Shaders/BlurUp");
	BindShader(blurUpShader);
	BlurConstants upSamplingBlurConstants = {};
	upSamplingBlurConstants.LerpT = 0.85f;
	upSamplingBlurConstants.NumSamples = 9;
	upSamplingBlurConstants.Samples[0] = { Vec2(-1.f, -1.f), 0.0625f };
	upSamplingBlurConstants.Samples[1] = { Vec2(-1.f, 0.f), 0.125f };
	upSamplingBlurConstants.Samples[2] = { Vec2(-1.f, 1.f), 0.0625f };
	upSamplingBlurConstants.Samples[3] = { Vec2(0.f, -1.f), 0.125f };
	upSamplingBlurConstants.Samples[4] = { Vec2(0.f, 0.f), 0.25f };
	upSamplingBlurConstants.Samples[5] = { Vec2(0.f, 1.f), 0.125f };
	upSamplingBlurConstants.Samples[6] = { Vec2(1.f, -1.f), 0.0625f };
	upSamplingBlurConstants.Samples[7] = { Vec2(1.f, 0.f), 0.125f };
	upSamplingBlurConstants.Samples[8] = { Vec2(1.f, 1.f), 0.0625f };

	BindTexture(m_blurDownRTVs[m_blurDownRTVs.size() - 1], 1);
	upSamplingBlurConstants.TexelSize = Vec2(1.f / (float)m_blurDownRTVs[m_blurDownRTVs.size() - 1]->m_dimensions.x, 1.f / (float)m_blurDownRTVs[m_blurDownRTVs.size() - 1]->m_dimensions.y);

	for (int rtvIndex = (int)m_blurUpRTVs.size() - 1; rtvIndex >= 0; rtvIndex--)
	{
		SetRTV(m_blurUpRTVs[rtvIndex]);
		BindTexture(m_blurDownRTVs[rtvIndex]);

		if (rtvIndex != (int)m_blurUpRTVs.size() - 1)
		{
			BindTexture(m_blurUpRTVs[rtvIndex + 1], 1);
			upSamplingBlurConstants.TexelSize = Vec2(1.f / (float)m_blurUpRTVs[rtvIndex + 1]->m_dimensions.x, 1.f / (float)m_blurUpRTVs[rtvIndex + 1]->m_dimensions.y);
		}

		CopyCPUToGPU((void*)&upSamplingBlurConstants, sizeof(upSamplingBlurConstants), m_blurCBO);
		BindConstantBuffer(k_blurConstantsSlot, m_blurCBO);

		viewport = {};
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.Width = (float)m_blurUpRTVs[rtvIndex]->m_dimensions.x;
		viewport.Height = (float)m_blurUpRTVs[rtvIndex]->m_dimensions.y;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_deviceContext->RSSetViewports(1, &viewport);
		DrawVertexBuffer(m_fullscreenVBO, 6);
	}

	SetRTV(m_blurredEmissiveRTV);
	BindTexture(m_emissiveRTV);
	BindTexture(m_blurUpRTVs[0], 1);
	upSamplingBlurConstants.TexelSize = Vec2(1.f / (float)m_blurUpRTVs[0]->m_dimensions.x, 1.f / (float)m_blurUpRTVs[0]->m_dimensions.y);
	CopyCPUToGPU((void*)&upSamplingBlurConstants, sizeof(upSamplingBlurConstants), m_blurCBO);
	BindConstantBuffer(k_blurConstantsSlot, m_blurCBO);

	viewport = {};
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;
	viewport.Width = (float)m_blurredEmissiveRTV->m_dimensions.x;
	viewport.Height = (float)m_blurredEmissiveRTV->m_dimensions.y;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_deviceContext->RSSetViewports(1, &viewport);
	DrawVertexBuffer(m_fullscreenVBO, 6);

	EndRenderEvent("Emissive Blur Up");


	//-------------------------------------------------------------------------------------------------------------------------------------------
	// Composite
	Shader* compositeShader = CreateOrGetShader("Data/Shaders/Composite");
	BindShader(compositeShader);

	BeginRenderEvent("Emissive Composite");
	
	SetRTV();
	BindTexture(m_blurredEmissiveRTV);
	SetBlendMode(BlendMode::ADDITIVE);
	DrawVertexBuffer(m_fullscreenVBO, 6);

	EndRenderEvent("Emissive Composite");

	EndRenderEvent("Emissive");
}

void Renderer::BeginRenderEvent(char const* eventName)
{
	int eventNameLength = (int)strlen(eventName) + 1;
	int eventNameWideCharLength = MultiByteToWideChar(CP_UTF8, 0, eventName, eventNameLength, NULL, 0);

	wchar_t* eventNameWideCharStr = new wchar_t[eventNameWideCharLength];
	MultiByteToWideChar(CP_UTF8, 0, eventName, eventNameLength, eventNameWideCharStr, eventNameWideCharLength);

	m_userDefinedAnnotations->BeginEvent(eventNameWideCharStr);
}

void Renderer::EndRenderEvent(char const* optional_eventName)
{
	UNUSED(optional_eventName);
	m_userDefinedAnnotations->EndEvent();
}
