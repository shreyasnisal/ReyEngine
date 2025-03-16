#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Spritesheet.hpp"

enum class TextBoxMode
{
	SHRINK_TO_FIT,
	OVERRUN
};

class BitmapFont
{
	friend class Renderer;

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture* fontTexture);

public:
	Texture* GetTexture() const;
	void AddVertsForText2D(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f);
	float GetTextWidth(float cellHeight, std::string const& text, float cellAspect = 1.f) const;
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& verts, AABB2 const& box, float cellHeight, std::string const& text, Rgba8 const& tint=Rgba8::WHITE, float cellAspect=1.f, Vec2 const& alignment=Vec2::ZERO, TextBoxMode mode=TextBoxMode::SHRINK_TO_FIT, int maxGlyphsToDraw=99999999);
	void AddVertsForText3D(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f, Vec2 const& alignment = Vec2(0.5f, 0.5f), int maxGlyphsToDraw = 9999999);

protected:
	float GetGlyphAspect(int glyphUnicode) const;

protected:
	std::string m_fontFilePathNameWithNoExtension;
	SpriteSheet m_fontGlyphsSpriteSheet;
};
