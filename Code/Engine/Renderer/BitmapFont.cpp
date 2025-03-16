#include "Engine/Renderer/BitmapFont.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/VertexUtils.hpp"

BitmapFont::BitmapFont(char const* fontFilePathNameWithNoExtension, Texture* fontTexture)
	: m_fontFilePathNameWithNoExtension(fontFilePathNameWithNoExtension)
	, m_fontGlyphsSpriteSheet(fontTexture, IntVec2(16, 16))
{
}

Texture* BitmapFont::GetTexture() const
{
	return m_fontGlyphsSpriteSheet.GetTexture();
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect)
{
	Vec2 characterMins = textMins;
	for (int characterIndex = 0; characterIndex < static_cast<int>(text.length()); characterIndex++)
	{
		AABB2 characterBox = AABB2(characterMins, characterMins + Vec2(cellHeight * cellAspect, cellHeight));
		AddVertsForAABB2(verts, characterBox, tint, m_fontGlyphsSpriteSheet.GetSpriteUVs(text[characterIndex]).m_mins, m_fontGlyphsSpriteSheet.GetSpriteUVs(text[characterIndex]).m_maxs);
		characterMins += Vec2(cellHeight * cellAspect, 0.f);
	}
}

float BitmapFont::GetTextWidth(float cellHeight, std::string const& text, float cellAspect) const
{
	return cellHeight * cellAspect * text.length();
}

// #TODO: What's this supposed to do?
float BitmapFont::GetGlyphAspect(int glyphUnicode) const
{
	UNUSED(glyphUnicode);
	
	return 1.f;
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& verts, AABB2 const& box, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, TextBoxMode mode, int maxGlyphsToDraw)
{
	Strings lines;
	int numLines = SplitStringOnDelimiter(lines, text, '\n');
	Vec2 textDimensions = Vec2::ZERO;
	for (int lineIndex = 0; lineIndex < numLines; lineIndex++)
	{
		float lineWidth = GetTextWidth(cellHeight, lines[lineIndex], cellAspect);
		if (lineWidth > textDimensions.x)
		{
			textDimensions.x = lineWidth;
		}
	}
	textDimensions.y = numLines * cellHeight;

	Vec2 boxDimensions = box.GetDimensions();

	if (mode == TextBoxMode::SHRINK_TO_FIT)
	{
		float scalingFactorX = boxDimensions.x / textDimensions.x;
		float scalingFactorY = boxDimensions.y / textDimensions.y;

		if (scalingFactorX <= scalingFactorY && scalingFactorX < 1.f)
		{
			cellHeight *= scalingFactorX;
			textDimensions *= scalingFactorX;
		}
		else if (scalingFactorY < scalingFactorX && scalingFactorY < 1.f)
		{
			cellHeight *= scalingFactorY;
			textDimensions *= scalingFactorY;
		}
	}

	Vec2 textStartPosition = box.m_mins + (boxDimensions - textDimensions) * alignment;

	int glyphsDrawn = 0;
	for (int lineIndex = 0; lineIndex < numLines; lineIndex++)
	{
		if (glyphsDrawn >= maxGlyphsToDraw)
		{
			break;
		}

		std::string textToDraw = lines[lineIndex];
		if (glyphsDrawn + static_cast<int>(lines[lineIndex].length()) > maxGlyphsToDraw)
		{
			textToDraw = textToDraw.substr(0, maxGlyphsToDraw - glyphsDrawn);
		}
		float lineWidth = GetTextWidth(cellHeight, lines[lineIndex], cellAspect);
		Vec2 lineStartPosition = textStartPosition + Vec2((textDimensions.x - lineWidth) * alignment.x, cellHeight * (numLines - lineIndex - 1));
		AddVertsForText2D(verts, lineStartPosition, cellHeight, textToDraw, tint, cellAspect);
		
		glyphsDrawn += static_cast<int>(lines[lineIndex].length());
	}
}

void BitmapFont::AddVertsForText3D(std::vector<Vertex_PCU>& verts, Vec2 const& textMins, float cellHeight, std::string const& text, Rgba8 const& tint, float cellAspect, Vec2 const& alignment, int maxGlyphsToDraw)
{
	AddVertsForTextInBox2D(verts, AABB2(Vec2::ZERO, Vec2::ONE), cellHeight, text, tint, cellAspect, Vec2::ZERO, TextBoxMode::OVERRUN, maxGlyphsToDraw);
	Vec3 origin = Vec3(0.f, textMins.x - GetTextWidth(cellHeight, text, cellAspect) * alignment.x, textMins.y - cellHeight * alignment.y);
	Mat44 transformMatrix = Mat44(Vec3::NORTH, Vec3::SKYWARD, Vec3::EAST, origin);
	TransformVertexArray3D(verts, transformMatrix);
}
