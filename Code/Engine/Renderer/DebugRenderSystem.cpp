#include "Engine/Renderer/DebugRenderSystem.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <mutex>

#ifndef POINT
	#undef POINT
#endif

#ifndef TEXT
	#undef TEXT
#endif

enum class DebugGeometryType
{
	INVALID = -1,

	POINT,
	LINE,
	ARROW,
	CYLINDER,
	SPHERE,
	BOX,
	PLANE,
	WORLD_TEXT,
	BILLBOARD_TEXT,
	SCREEN_TEXT,
	MESSAGE,

	COUNT
};

struct DebugGeometry
{
public:
	DebugGeometryType m_type = DebugGeometryType::INVALID;
	std::vector<Vertex_PCU> m_vertexes;
	Rgba8 m_startColor = Rgba8::WHITE;
	Rgba8 m_endColor = Rgba8::WHITE;
	RasterizerFillMode m_rasterizerFillMode = RasterizerFillMode::SOLID;
	RasterizerCullMode m_rasterizerCullMode = RasterizerCullMode::CULL_BACK;
	BlendMode m_blendMode = BlendMode::OPAQUE;
	DebugRenderMode m_debugRenderMode = DebugRenderMode::USE_DEPTH;
	Stopwatch* m_durationTimer = nullptr;
	Texture* m_texture = nullptr;
	Vec3 m_translation = Vec3::ZERO;
	BillboardType m_billboardType = BillboardType::FULL_OPPOSING;
};

std::mutex s_debugWorldGeometryMutex;
static std::vector<DebugGeometry> s_debugWorldGeometries;

std::mutex s_debugScreenGeometryMutex;
static std::vector<DebugGeometry> s_debugScreenGeometries;

struct DebugMessage
{
public:
	std::string				m_text = "";
	Stopwatch* m_durationTimer = nullptr;
	Rgba8					m_startColor = Rgba8::WHITE;
	Rgba8					m_endColor = Rgba8::WHITE;
};

std::mutex s_debugMessageMutex;
static std::vector<DebugMessage> s_debugMessages;

static DebugRenderConfig s_config;
static std::atomic<bool> s_isDebugRenderVisible = true;
//static Clock s_debugRenderSystemClock = Clock();
static BitmapFont* s_debugRenderSystemFont = nullptr;

void DebugRenderSystemStartup(DebugRenderConfig const& config)
{
	s_config = config;
	s_debugRenderSystemFont = s_config.m_renderer->CreateOrGetBitmapFont(s_config.m_bitmapFontFilePathWithNoExtension.c_str());

	if (!s_config.m_startVisible)
	{
		s_isDebugRenderVisible = false;
	}

	SubscribeEventCallbackFunction("DebugRenderClear", Command_DebugRenderClear, "Clear all debug elements");
	SubscribeEventCallbackFunction("DebugRenderToggle", Command_DebugRenderToggle, "Toggles debug element visibility");
}

void DebugRenderSystemShutdown()
{
}

void DebugRenderBeginFrame()
{
	s_debugWorldGeometryMutex.lock();
	for (int worldGeometryIndex = 0; worldGeometryIndex < (int)s_debugWorldGeometries.size(); worldGeometryIndex++)
	{
		Stopwatch* const& worldGeomtryTimer = s_debugWorldGeometries[worldGeometryIndex].m_durationTimer;
		if (worldGeomtryTimer && worldGeomtryTimer->HasDurationElapsed())
		{
			s_debugWorldGeometries.erase(s_debugWorldGeometries.begin() + worldGeometryIndex);
			worldGeometryIndex--;
		}
	}
	s_debugWorldGeometryMutex.unlock();

	s_debugScreenGeometryMutex.lock();
	for (int screenGeometryIndex = 0; screenGeometryIndex < (int)s_debugScreenGeometries.size(); screenGeometryIndex++)
	{
		Stopwatch* const& screenGeometryTimer = s_debugScreenGeometries[screenGeometryIndex].m_durationTimer;
		if (screenGeometryTimer && screenGeometryTimer->HasDurationElapsed())
		{
			s_debugScreenGeometries.erase(s_debugScreenGeometries.begin() + screenGeometryIndex);
			screenGeometryIndex--;
		}
	}
	s_debugScreenGeometryMutex.unlock();

	s_debugMessageMutex.lock();
	for (int debugMessageIndex = 0; debugMessageIndex < (int)s_debugMessages.size(); debugMessageIndex++)
	{
		Stopwatch* const& debugMessageTimer = s_debugMessages[debugMessageIndex].m_durationTimer;
		if (debugMessageTimer && debugMessageTimer->HasDurationElapsed())
		{
			s_debugMessages.erase(s_debugMessages.begin() + debugMessageIndex);
			debugMessageIndex--;
		}
	}
	s_debugMessageMutex.unlock();

}

void DebugRenderWorld(Camera const& camera)
{
	if (!s_isDebugRenderVisible)
	{
		return;
	}

	s_config.m_renderer->BeginCamera(camera);
	s_config.m_renderer->BeginRenderEvent("Debug Render World");
	s_debugWorldGeometryMutex.lock();

	for (int worldGeometryIndex = 0; worldGeometryIndex < (int)s_debugWorldGeometries.size(); worldGeometryIndex++)
	{
		DebugGeometry const& worldGeometry = s_debugWorldGeometries[worldGeometryIndex];
		Rgba8 currentColor = worldGeometry.m_durationTimer ? Interpolate(worldGeometry.m_startColor, worldGeometry.m_endColor, worldGeometry.m_durationTimer->GetElapsedFraction()) : worldGeometry.m_startColor;

		DepthMode depthMode = DepthMode::ENABLED;

		if (worldGeometry.m_debugRenderMode == DebugRenderMode::ALWAYS)
		{
			continue;
		}
		else if (worldGeometry.m_debugRenderMode == DebugRenderMode::X_RAY)
		{
			continue;
		}

		Mat44 transform = Mat44();
		if (worldGeometry.m_type == DebugGeometryType::BILLBOARD_TEXT)
		{
			transform = GetBillboardMatrix(worldGeometry.m_billboardType, camera.GetModelMatrix(), worldGeometry.m_translation);
		}

		s_config.m_renderer->SetDepthMode(depthMode);
		s_config.m_renderer->SetBlendMode(worldGeometry.m_blendMode);
		s_config.m_renderer->BindTexture(worldGeometry.m_texture);
		s_config.m_renderer->SetRasterizerFillMode(worldGeometry.m_rasterizerFillMode);
		s_config.m_renderer->SetRasterizerCullMode(worldGeometry.m_rasterizerCullMode);
		s_config.m_renderer->SetModelConstants(transform, currentColor);
		s_config.m_renderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
		s_config.m_renderer->BindShader(nullptr);
		s_config.m_renderer->DrawVertexArray(worldGeometry.m_vertexes);
	}

	// Draw XRay and Depth Disabled stuff here
	for (int worldGeometryIndex = 0; worldGeometryIndex < (int)s_debugWorldGeometries.size(); worldGeometryIndex++)
	{
		DebugGeometry const& worldGeometry = s_debugWorldGeometries[worldGeometryIndex];
		Rgba8 currentColor = worldGeometry.m_durationTimer ? Interpolate(worldGeometry.m_startColor, worldGeometry.m_endColor, worldGeometry.m_durationTimer->GetElapsedFraction()) : worldGeometry.m_startColor;

		DepthMode depthMode = DepthMode::ENABLED;

		if (worldGeometry.m_debugRenderMode == DebugRenderMode::ALWAYS)
		{
			depthMode = DepthMode::DISABLED;
		}
		else if (worldGeometry.m_debugRenderMode == DebugRenderMode::X_RAY)
		{
			Rgba8 xrayColor = currentColor;
			xrayColor.MultiplyRGBScaled(Rgba8::WHITE, 0.1f);
			xrayColor = Rgba8(xrayColor.r, xrayColor.g, xrayColor.b, (unsigned char)RoundDownToInt(xrayColor.a * 0.5f));
			s_config.m_renderer->SetDepthMode(DepthMode::DISABLED);
			s_config.m_renderer->SetBlendMode(BlendMode::ALPHA);
			s_config.m_renderer->BindTexture(worldGeometry.m_texture);
			s_config.m_renderer->SetRasterizerFillMode(worldGeometry.m_rasterizerFillMode);
			s_config.m_renderer->SetRasterizerCullMode(RasterizerCullMode::CULL_BACK);
			s_config.m_renderer->SetModelConstants(Mat44(), xrayColor);
			s_config.m_renderer->BindShader(nullptr);
			s_config.m_renderer->DrawVertexArray(worldGeometry.m_vertexes);
		}
		else
		{
			continue;
		}

		Mat44 transform = Mat44();
		if (worldGeometry.m_type == DebugGeometryType::BILLBOARD_TEXT)
		{
			transform = GetBillboardMatrix(worldGeometry.m_billboardType, camera.GetModelMatrix(), worldGeometry.m_translation);
		}

		s_config.m_renderer->SetDepthMode(depthMode);
		s_config.m_renderer->SetBlendMode(worldGeometry.m_blendMode);
		s_config.m_renderer->BindTexture(worldGeometry.m_texture);
		s_config.m_renderer->SetRasterizerFillMode(worldGeometry.m_rasterizerFillMode);
		s_config.m_renderer->SetRasterizerCullMode(worldGeometry.m_rasterizerCullMode);
		s_config.m_renderer->SetModelConstants(transform, currentColor);
		s_config.m_renderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
		s_config.m_renderer->BindShader(nullptr);
		s_config.m_renderer->DrawVertexArray(worldGeometry.m_vertexes);
	}

	s_debugWorldGeometryMutex.unlock();
	s_config.m_renderer->EndCamera(camera);
	s_config.m_renderer->EndRenderEvent();
}

void DebugRenderScreen(Camera const& camera)
{
	if (!s_isDebugRenderVisible)
	{
		return;
	}

	s_config.m_renderer->BeginCamera(camera);
	s_config.m_renderer->BeginRenderEvent("Debug Render Screen");
	s_debugScreenGeometryMutex.lock();

	for (int screenGeometryIndex = 0; screenGeometryIndex < (int)s_debugScreenGeometries.size(); screenGeometryIndex++)
	{
		DebugGeometry const& screenGeometry = s_debugScreenGeometries[screenGeometryIndex];
		Rgba8 currentColor = screenGeometry.m_durationTimer ? Interpolate(screenGeometry.m_startColor, screenGeometry.m_endColor, screenGeometry.m_durationTimer->GetElapsedFraction()) : screenGeometry.m_startColor;

		s_config.m_renderer->SetDepthMode(DepthMode::DISABLED);
		s_config.m_renderer->SetBlendMode(BlendMode::ALPHA);
		s_config.m_renderer->BindTexture(screenGeometry.m_texture);
		s_config.m_renderer->SetRasterizerFillMode(screenGeometry.m_rasterizerFillMode);
		s_config.m_renderer->SetRasterizerCullMode(screenGeometry.m_rasterizerCullMode);
		s_config.m_renderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
		s_config.m_renderer->SetModelConstants(Mat44(), currentColor);
		s_config.m_renderer->BindShader(nullptr);
		s_config.m_renderer->DrawVertexArray(screenGeometry.m_vertexes);
	}
	s_debugScreenGeometryMutex.unlock();

	std::vector<Vertex_PCU> textVerts;
	// Render messages with infinite duration
	float messageHeight = (camera.GetOrthoTopRight().y - camera.GetOrthoBottomLeft().y) * s_config.m_messageHeightFractionOfScreenHeight;
	Vec2 messageTextMins = Vec2((camera.GetOrthoTopRight().x - camera.GetOrthoBottomLeft().x) * 0.01f, camera.GetOrthoTopRight().y - messageHeight);
	s_debugMessageMutex.lock();
	for (int messageIndex = (int)(s_debugMessages.size()) - 1; messageIndex >= 0; messageIndex--)
	{
		DebugMessage const& message = s_debugMessages[messageIndex];
		if (message.m_durationTimer != nullptr)
		{
			continue;
		}
		Rgba8 textColor = message.m_durationTimer ? Interpolate(message.m_startColor, message.m_endColor, message.m_durationTimer->GetElapsedFraction()) : message.m_startColor;
		messageTextMins.y -= messageHeight;
		if (messageTextMins.y < camera.GetOrthoBottomLeft().y)
		{
			// Messages are going below the screen bounds, skip displaying more messages
			break;
		}
		s_debugRenderSystemFont->AddVertsForText2D(textVerts, messageTextMins, messageHeight, message.m_text, textColor, 0.7f);
	}

	// Render messages with finite duration
	for (int messageIndex = (int)(s_debugMessages.size()) - 1; messageIndex >= 0; messageIndex--)
	{
		DebugMessage const& message = s_debugMessages[messageIndex];
		if (message.m_durationTimer == nullptr)
		{
			continue;
		}
		Rgba8 textColor = message.m_durationTimer ? Interpolate(message.m_startColor, message.m_endColor, message.m_durationTimer->GetElapsedFraction()) : message.m_startColor;
		messageTextMins.y -= messageHeight;
		if (messageTextMins.y < camera.GetOrthoBottomLeft().y)
		{
			// Messages are going below the screen bounds, skip displaying more messages
			break;
		}
		s_debugRenderSystemFont->AddVertsForText2D(textVerts, messageTextMins, messageHeight, message.m_text, textColor, 0.7f);
	}

	s_debugMessageMutex.unlock();

	s_config.m_renderer->SetDepthMode(DepthMode::DISABLED);
	s_config.m_renderer->SetBlendMode(BlendMode::ALPHA);
	s_config.m_renderer->BindTexture(s_debugRenderSystemFont->GetTexture());
	s_config.m_renderer->SetRasterizerFillMode(RasterizerFillMode::SOLID);
	s_config.m_renderer->SetRasterizerCullMode(RasterizerCullMode::CULL_NONE);
	s_config.m_renderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
	s_config.m_renderer->BindShader(nullptr);
	s_config.m_renderer->SetModelConstants();
	s_config.m_renderer->DrawVertexArray(textVerts);

	s_config.m_renderer->EndCamera(camera);
	s_config.m_renderer->EndRenderEvent();
}

void DebugRenderSetVisible()
{
	s_isDebugRenderVisible = true;
}

void DebugRenderSetHidden()
{
	s_isDebugRenderVisible = false;
}

void DebugRenderClear()
{
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.clear();
	s_debugWorldGeometryMutex.unlock();

	s_debugScreenGeometryMutex.lock();
	s_debugScreenGeometries.clear();
	s_debugScreenGeometryMutex.unlock();

	s_debugMessageMutex.lock();
	s_debugMessages.clear();
	s_debugMessageMutex.unlock();
}

void DebugRenderEndFrame()
{
}

void DebugAddWorldPoint(Vec3 const& position, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry point;
	point.m_type = DebugGeometryType::POINT;
	AddVertsForSphere3D(point.m_vertexes, position, radius);
	if (duration != -1.f)
	{
		point.m_durationTimer = new Stopwatch(duration);
		point.m_durationTimer->Start();
	}
	point.m_startColor = startColor;
	point.m_endColor = endColor;
	point.m_debugRenderMode = mode;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(point);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldLine(Vec3 const& startPosition, Vec3 const& endPosition, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry line;
	line.m_type = DebugGeometryType::LINE;
	AddVertsForCylinder3D(line.m_vertexes, startPosition, endPosition, radius);
	if (duration != -1.f)
	{
		line.m_durationTimer = new Stopwatch(duration);
		line.m_durationTimer->Start();
	}
	line.m_startColor = startColor;
	line.m_endColor = endColor;
	line.m_debugRenderMode = mode;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(line);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldArrow(Vec3 const& startPosition, Vec3 const& endPosition, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry arrow;
	arrow.m_type = DebugGeometryType::ARROW;
	AddVertsForArrow3D(arrow.m_vertexes, startPosition, endPosition, radius);
	if (duration != -1.f)
	{
		arrow.m_durationTimer = new Stopwatch(duration);
		arrow.m_durationTimer->Start();
	}
	arrow.m_startColor = startColor;
	arrow.m_endColor = endColor;
	arrow.m_debugRenderMode = mode;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(arrow);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldWireframePlane(Plane3 const& plane, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry planeDebugGeometry;
	planeDebugGeometry.m_type = DebugGeometryType::PLANE;
	AddVertsForWireframePlane3(planeDebugGeometry.m_vertexes, plane);
	if (duration != -1.f)
	{
		planeDebugGeometry.m_durationTimer = new Stopwatch(duration);
		planeDebugGeometry.m_durationTimer->Start();
	}
	planeDebugGeometry.m_startColor = startColor;
	planeDebugGeometry.m_endColor = endColor;
	planeDebugGeometry.m_debugRenderMode = mode;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(planeDebugGeometry);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldWireCylinder(Vec3 const& baseCenter, Vec3 const& topCenter, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry cylinder;
	cylinder.m_type = DebugGeometryType::CYLINDER;
	AddVertsForCylinder3D(cylinder.m_vertexes, baseCenter, topCenter, radius);
	if (duration != -1.f)
	{
		cylinder.m_durationTimer = new Stopwatch(duration);
		cylinder.m_durationTimer->Start();
	}
	cylinder.m_startColor = startColor;
	cylinder.m_endColor = endColor;
	cylinder.m_debugRenderMode = mode;
	cylinder.m_rasterizerFillMode = RasterizerFillMode::WIREFRAME;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(cylinder);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldCylinder(Vec3 const& baseCenter, Vec3 const& topCenter, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry cylinder;
	cylinder.m_type = DebugGeometryType::CYLINDER;
	AddVertsForCylinder3D(cylinder.m_vertexes, baseCenter, topCenter, radius);
	if (duration != -1.f)
	{
		cylinder.m_durationTimer = new Stopwatch(duration);
		cylinder.m_durationTimer->Start();
	}
	cylinder.m_startColor = startColor;
	cylinder.m_endColor = endColor;
	cylinder.m_debugRenderMode = mode;
	cylinder.m_rasterizerFillMode = RasterizerFillMode::SOLID;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(cylinder);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldWireSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry sphere;
	sphere.m_type = DebugGeometryType::SPHERE;
	AddVertsForSphere3D(sphere.m_vertexes, center, radius);
	if (duration != -1.f)
	{
		sphere.m_durationTimer = new Stopwatch(duration);
		sphere.m_durationTimer->Start();
	}
	sphere.m_startColor = startColor;
	sphere.m_endColor = endColor;
	sphere.m_debugRenderMode = mode;
	sphere.m_rasterizerFillMode = RasterizerFillMode::WIREFRAME;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(sphere);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldWireBox(AABB3 const& box, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry boxGeometry;
	boxGeometry.m_type = DebugGeometryType::BOX;
	AddVertsForAABB3(boxGeometry.m_vertexes, box, Rgba8::WHITE);
	if (duration != -1.f)
	{
		boxGeometry.m_durationTimer = new Stopwatch(duration);
		boxGeometry.m_durationTimer->Start();
	}
	boxGeometry.m_startColor = startColor;
	boxGeometry.m_endColor = endColor;
	boxGeometry.m_debugRenderMode = mode;
	boxGeometry.m_rasterizerFillMode = RasterizerFillMode::WIREFRAME;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(boxGeometry);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldWireBox(OBB3 const& box, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry boxGeometry;
	boxGeometry.m_type = DebugGeometryType::BOX;
	AddVertsForOBB3(boxGeometry.m_vertexes, box, Rgba8::WHITE);
	if (duration != -1.f)
	{
		boxGeometry.m_durationTimer = new Stopwatch(duration);
		boxGeometry.m_durationTimer->Start();
	}
	boxGeometry.m_startColor = startColor;
	boxGeometry.m_endColor = endColor;
	boxGeometry.m_debugRenderMode = mode;
	boxGeometry.m_rasterizerFillMode = RasterizerFillMode::WIREFRAME;
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(boxGeometry);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddWorldText(std::string const& text, Mat44 const& transform, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	DebugGeometry worldText;
	worldText.m_type = DebugGeometryType::WORLD_TEXT;
	s_debugRenderSystemFont->AddVertsForText3D(worldText.m_vertexes, Vec2::ZERO, textHeight, text, Rgba8::WHITE, 1.f, alignment);
	TransformVertexArray3D(worldText.m_vertexes, transform);
	if (duration != -1.f)
	{
		worldText.m_durationTimer = new Stopwatch(duration);
		worldText.m_durationTimer->Start();
	}
	worldText.m_startColor = startColor;
	worldText.m_endColor = endColor;
	worldText.m_debugRenderMode = mode;
	worldText.m_blendMode = BlendMode::ALPHA;
	worldText.m_rasterizerCullMode = RasterizerCullMode::CULL_NONE;
	worldText.m_texture = s_debugRenderSystemFont->GetTexture();
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(worldText);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddBillboardText(std::string const& text, Vec3 const& origin, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode, BillboardType billboardType)
{
	DebugGeometry billboardText;
	billboardText.m_type = DebugGeometryType::BILLBOARD_TEXT;
	s_debugRenderSystemFont->AddVertsForText3D(billboardText.m_vertexes, Vec2::ZERO, textHeight, text, Rgba8::WHITE, 1.f, alignment);
	billboardText.m_translation = origin;
	if (duration != -1.f)
	{
		billboardText.m_durationTimer = new Stopwatch(duration);
		billboardText.m_durationTimer->Start();
	}
	billboardText.m_startColor = startColor;
	billboardText.m_endColor = endColor;
	billboardText.m_debugRenderMode = mode;
	billboardText.m_blendMode = BlendMode::ALPHA;
	billboardText.m_rasterizerCullMode = RasterizerCullMode::CULL_NONE;
	billboardText.m_billboardType = billboardType;
	billboardText.m_texture = s_debugRenderSystemFont->GetTexture();
	s_debugWorldGeometryMutex.lock();
	s_debugWorldGeometries.push_back(billboardText);
	s_debugWorldGeometryMutex.unlock();
}

void DebugAddScreenText(std::string const& text, Vec2 const& position, float size, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor)
{
	DebugGeometry screenText;
	screenText.m_type = DebugGeometryType::SCREEN_TEXT;
	s_debugRenderSystemFont->AddVertsForTextInBox2D(screenText.m_vertexes, AABB2(position, position), size, text, Rgba8::WHITE, 0.7f, alignment, TextBoxMode::OVERRUN);
	if (duration != -1.f)
	{
		screenText.m_durationTimer = new Stopwatch(duration);
		screenText.m_durationTimer->Start();
	}
	screenText.m_startColor = startColor;
	screenText.m_endColor = endColor;
	screenText.m_blendMode = BlendMode::ALPHA;
	screenText.m_rasterizerCullMode = RasterizerCullMode::CULL_BACK;
	screenText.m_texture = s_debugRenderSystemFont->GetTexture();
	s_debugScreenGeometryMutex.lock();
	s_debugScreenGeometries.push_back(screenText);
	s_debugScreenGeometryMutex.unlock();
}

void DebugAddMessage(std::string const& text, float duration, Rgba8 const& startColor, Rgba8 const& endColor)
{
	DebugMessage message;
	message.m_text = text;
	if (duration != -1.f)
	{
		message.m_durationTimer = new Stopwatch(duration);
		message.m_durationTimer->Start();
	}
	message.m_startColor = startColor;
	message.m_endColor = endColor;
	s_debugMessageMutex.lock();
	s_debugMessages.push_back(message);
	s_debugMessageMutex.unlock();
}

bool Command_DebugRenderClear(EventArgs& args)
{
	UNUSED(args);

	DebugRenderClear();
	return true;
}

bool Command_DebugRenderToggle(EventArgs& args)
{
	UNUSED(args);

	if (s_isDebugRenderVisible)
	{
		DebugRenderSetHidden();
		s_isDebugRenderVisible = false;
	}
	else
	{
		DebugRenderSetHidden();
		s_isDebugRenderVisible = true;
	}

	return true;
}
