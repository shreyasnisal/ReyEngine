#include "Engine/Core/Image.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

/*! \brief Constructor to create an Image from an image on disc
* 
* \param imageFilePath The path to the image followed by the name of the image including the extension
* 
*/
Image::Image(char const* imageFilePath)
	: m_imageFilePath(imageFilePath)
{
	FILE* filePtr;
	fopen_s(&filePtr, imageFilePath, "r");
	if (!filePtr)
	{
		ERROR_AND_DIE(Stringf("Could not find file \"%s\"", imageFilePath));
	}

	int bytesPerTexel = 0;
	int numComponentsRequested = 0;

	stbi_set_flip_vertically_on_load(1);
	unsigned char* texelData = stbi_load(imageFilePath, &m_dimensions.x, &m_dimensions.y, &bytesPerTexel, numComponentsRequested);
	m_rgbaTexels.reserve(m_dimensions.x * m_dimensions.y);

	for (int texelIndex = 0; texelIndex < m_dimensions.x * m_dimensions.y * bytesPerTexel; texelIndex += bytesPerTexel)
	{
		unsigned char redByte = texelData[texelIndex];
		unsigned char greenByte = texelData[texelIndex + 1];
		unsigned char blueByte = texelData[texelIndex + 2];
		unsigned char alphaByte = (bytesPerTexel == 4 ? texelData[texelIndex + 3] : 255);
		m_rgbaTexels.emplace_back(Rgba8(redByte, greenByte, blueByte, alphaByte));
	}

	stbi_image_free(texelData);
}

/*! \brief Constructor to create an Image based on dimensions and a color
* 
* Resizes the list of Rgba8 colors to dimensions.x * dimensions.y and fills all values with the color provided
* \param size An IntVec2 containing the dimensions of the image to create
* \param color The Rgba8 color to be applied to all texels in the Image
* 
*/
Image::Image(IntVec2 const& size, Rgba8 const& color)
	: m_dimensions(size)
{
	for (int texelIndex = 0; texelIndex < size.x * size.y; texelIndex++)
	{
		m_rgbaTexels.push_back(color);
	}
}

/*! \brief Gets the color for a single texel in the Image
* 
* \param texelCoords An IntVec2 containing the coordinates of the texel for which the color should be retrieved
* \return An Rgba8 containing the color of that texel
* 
*/
Rgba8 Image::GetTexelColor(IntVec2 const& texelCoords)
{
	int texelIndex = texelCoords.x + texelCoords.y * m_dimensions.x;
	return m_rgbaTexels[texelIndex];
}

/*! \brief Sets the color for a single texel in the Image
* 
* \param texelCoords An IntVec2 containing the coordinates of the texel for which the color should be applied
* \param newColor The Rgba8 color that should be applied to the provided texel
* 
*/
void Image::SetTexelColor(IntVec2 const& texelCoords, Rgba8 const& newColor)
{
	int texelIndex = texelCoords.x + texelCoords.y * m_dimensions.x;
	m_rgbaTexels[texelIndex] = newColor;
}
