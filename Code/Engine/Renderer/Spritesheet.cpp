#include "Engine/Renderer/Spritesheet.hpp"

#include "Engine/Math/MathUtils.hpp"


SpriteSheet::SpriteSheet(Texture* texture, IntVec2 const& simpleGridLayout)
	: m_texture(texture)
{
	IntVec2 textureDimensions = texture->GetDimensions();
	int numSprites = simpleGridLayout.x * simpleGridLayout.y;

	for (int spriteIndex = 0; spriteIndex < numSprites; spriteIndex++)
	{
		float minsX = static_cast<float>(spriteIndex % simpleGridLayout.x);
		float minsY = static_cast<float>(simpleGridLayout.y) - 1.f - static_cast<float>(spriteIndex / simpleGridLayout.x);

		float samplingCorrectionFactorX = 1.f / (128.f * textureDimensions.x);
		float samplingCorrectionFactorY = 1.f / (128.f * textureDimensions.y);

		float uvAtMinsX =  minsX / static_cast<float>(simpleGridLayout.x);
		float uvAtMaxsX =  uvAtMinsX + (1.f / static_cast<float>(simpleGridLayout.x));
		float uvAtMinsY =  minsY / static_cast<float>(simpleGridLayout.y);
		float uvAtMaxsY = uvAtMinsY + (1.f / static_cast<float>(simpleGridLayout.y));

		uvAtMinsX += samplingCorrectionFactorX;
		uvAtMaxsX -= samplingCorrectionFactorX;
		uvAtMinsY += samplingCorrectionFactorY;
		uvAtMaxsY -= samplingCorrectionFactorY;

		m_spriteDefs.push_back(SpriteDefinition(this, spriteIndex, Vec2(uvAtMinsX, uvAtMinsY), Vec2(uvAtMaxsX, uvAtMaxsY)));
	}
}
