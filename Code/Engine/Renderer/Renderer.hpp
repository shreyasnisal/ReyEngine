#pragma once

#include "Game/EngineBuildPreferences.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/SurfaceData.hpp"

#include <vector>

#define DX_SAFE_RELEASE(dxObject)					\
{													\
	if ((dxObject) != nullptr)						\
	{												\
		(dxObject)->Release();						\
		(dxObject) = nullptr;						\
	}												\
}

#if defined(OPAQUE)
	#undef OPAQUE
#endif

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;
struct ID3D11Resource;
struct ID3D11Texture2D;
struct ID3DUserDefinedAnnotation;

class BitmapFont;
class ConstantBuffer;
class Image;
class Material;
struct Model;
class Shader;
class Texture;
class IndexBuffer;
class VertexBuffer;
class OpenXR;

struct RenderConfig
{
public:
	Window* m_window = nullptr;
	bool m_emissiveEnabled = false;
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUE,

	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	BILINEAR_CLAMP,

	COUNT
};

enum class RasterizerFillMode
{
	SOLID,
	WIREFRAME,

	COUNT
};

enum class RasterizerCullMode
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,

	COUNT
};

enum class DepthMode
{
	DISABLED,
	ENABLED,

	READ_ONLY_LESS_EQUAL,

	COUNT,
};

struct LightingDebug
{
public:
	int RenderAmbientDebugFlag = true;
	int RenderDiffuseFlag = true;
	int RenderSpecularDebugFlag = true;
	int RenderEmissiveDebugFlag = true;
	int UseDiffuseMapDebugFlag = true;
	int UseNormalMapDebugFlag = true;
	int UseSpecularMapDebugFlag = true;
	int UseGlossinessMapDebugFlag = true;
	int UseEmissiveMapDebugFlag = true;
	float padding[3];
};

struct LightConstants
{
	Vec3 SunDirection = Vec3::ZERO;
	float SunIntensity = 1.f;
	float AmbientIntensity = 0.f;
	float padding0[3];
	Mat44 LightViewMatrix = Mat44::IDENTITY;
	Mat44 LightProjectionMatrix = Mat44::IDENTITY;
	Vec3 WorldEyePosition = Vec3::ZERO;
	float MinimumFalloff = 0.f;
	float MaximumFalloff = 1.f;
	float MinimumFalloffMultiplier = 0.f;
	float MaximumFalloffMultiplier = 1.f;
	float padding1;

	LightingDebug lightingDebug;
};

class Renderer
{
public:
	~Renderer() = default;
	Renderer(RenderConfig config);

	void					Startup();
	void					BeginFrame();
	void					EndFrame();
	void					Shutdown();
	void					BeginRenderForEye(XREye eye = XREye::NONE);

	void					CreateRasterizerStates();
	SurfaceData				CreateSurface(int width, int height, ID3D11Texture2D* texture, bool isOpenXRTexture = false);

	void					ClearScreen(const Rgba8& clearColor);
	void					ClearRTV(Rgba8 const& clearColor, Texture* texture = nullptr);
	void					ClearDSV(Texture* texture = nullptr);
	void					BeginCamera(const Camera& camera);
	void					EndCamera(const Camera& camera);
	void					SetRTV(Texture* texture = nullptr);
	void					SetDSV(Texture* texture = nullptr);
	void					BindDepthBuffer(Texture* texture);
	void					BindTexture(Texture const* texture, int textureSlot = 0);
	void					SetBlendMode(BlendMode blendMode);
	void					SetSamplerMode(SamplerMode samplerMode);
	void					SetModelConstants(Mat44 const& modelMatrix = Mat44(), Rgba8 const& modelColor = Rgba8::WHITE);
	void					SetLightConstants(Vec3 const& lightPosition, float lightIntensity, float ambientLightIntensity, Vec3 const& worldEyePosition = Vec3::ZERO);
	void					SetLightConstants(LightConstants const& lightConstants);
	void					SetRasterizerFillMode(RasterizerFillMode fillMode);
	void					SetRasterizerCullMode(RasterizerCullMode cullMode);
	void					SetDepthMode(DepthMode depthMode);
	void					SetStatesIfChanged();
	void					DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes);
	void					DrawVertexArray(std::vector<Vertex_PCU> const& vertexes);
	void					DrawVertexArray(std::vector<Vertex_PCUTBN> const& vertexes);
	void					DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, int vertexOffset = 0);
	void					DrawIndexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, int indexCount);

	Texture*				CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture*				GetTextureFromFileName(char const* name);
	Texture*				CreateTextureFromFile(char const* imageFilePath);
	Texture*				CreateTextureFromImage(char const* name, Image const& image);
	Texture*				CreateRenderTargetTexture(char const* name, IntVec2 const& dimensions);
	Texture*				CreateDepthBuffer(char const* name, IntVec2 const& dimensions);

	BitmapFont*				CreateOrGetBitmapFont(const char* bitmapFontFilePathWithNoExtension);
	BitmapFont*				GetBitmapFontFromFileName(char const* bitmapFontName);
	BitmapFont*				CreateBitmapFromFile(char const* bitmapFontFilePathWithNoExtension);

	Shader*					CreateOrGetShader(char const* shaderName, VertexType vertexType = VertexType::VERTEX_PCU);
	Shader*					GetShaderFromFileName(char const* shaderName);
	Shader*					CreateShader(char const* shaderName, VertexType vertexType = VertexType::VERTEX_PCU);
	Shader*					CreateShader(char const* shaderName, char const* shaderSource, VertexType vertexType = VertexType::VERTEX_PCU);
	bool					CompileShaderToByteCode(std::vector<unsigned char>& out_byteCode, char const* name, char const* source, char const* entryPoint, char const* target);
	void					BindShader(Shader* shader);

	VertexBuffer*			CreateVertexBuffer(size_t const size, VertexType vertexType = VertexType::VERTEX_PCU, bool isLinePrimitive = false) const;
	void					CopyCPUToGPU(const void* data, size_t size, VertexBuffer*& vbo) const;
	void					BindVertexBuffer(VertexBuffer* vbo);

	ConstantBuffer*			CreateConstantBuffer(size_t const size) const;
	void					CopyCPUToGPU(const void* data, size_t size, ConstantBuffer* cbo) const;
	void					BindConstantBuffer(int slot, ConstantBuffer* cbo);

	IndexBuffer*			CreateIndexBuffer(size_t const size) const;
	void					CopyCPUToGPU(const void* data, size_t size, IndexBuffer*& ibo) const;
	void					BindIndexBuffer(IndexBuffer* ibo);

	ID3D11Device*			GetDevice() { return m_device; }
	ID3D11DeviceContext*	GetDeviceContext() { return m_deviceContext; }

	void					RenderEmissive();

	void					BeginRenderEvent(char const* eventName);
	void					EndRenderEvent(char const* optional_eventName = nullptr);

public:

protected:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_backBufferRTV = nullptr;

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	RasterizerFillMode m_desiredRasterizerFillMode = RasterizerFillMode::SOLID;
	RasterizerCullMode m_desiredRasterizerCullMode = RasterizerCullMode::CULL_BACK;
	ID3D11RasterizerState* m_rasterizerStates[(int)(RasterizerFillMode::COUNT) * (int)(RasterizerCullMode::COUNT)] = {};
	
	void* m_dxgiDebugModule = nullptr;
	void* m_dxgiDebug = nullptr;

	std::vector<Shader*> m_loadedShaders;
	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;

	VertexBuffer* m_immediateVBO = nullptr;

	ConstantBuffer* m_cameraCBO = nullptr;
	ConstantBuffer* m_modelCBO = nullptr;
	ConstantBuffer* m_lightCBO = nullptr;
	ConstantBuffer* m_blurCBO = nullptr;

	ID3D11BlendState* m_blendState = nullptr;
	BlendMode m_desiredBlendMode = BlendMode::ALPHA;
	ID3D11BlendState* m_blendStates[(int) (BlendMode::COUNT)] = {};

	ID3D11SamplerState* m_samplerState = nullptr;
	SamplerMode m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	ID3D11SamplerState* m_samplerStates[(int)(SamplerMode::COUNT)] = {};
	ID3D11SamplerState* m_depthSamplerComparisonState = nullptr;

	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	DepthMode m_desiredDepthStencilMode = DepthMode::ENABLED;
	ID3D11DepthStencilState* m_depthStencilStates[(int) (DepthMode::COUNT)] = {};
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_depthStencilTexture = nullptr;

	Texture const* m_defaultTexture = nullptr;

	VertexBuffer* m_fullscreenVBO = nullptr;
	Texture* m_emissiveRTV = nullptr;
	Texture* m_blurredEmissiveRTV = nullptr;
	std::vector<Texture*> m_blurDownRTVs;
	std::vector<Texture*> m_blurUpRTVs;

	ID3DUserDefinedAnnotation* m_userDefinedAnnotations = nullptr;

private:
	RenderConfig				m_config;
	std::vector<Texture*>		m_loadedTextures; 
	std::vector<BitmapFont*>	m_loadedFonts;
	XREye m_currentEye = XREye::NONE;
};
