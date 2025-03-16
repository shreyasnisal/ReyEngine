#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/IntVec2.hpp"

#include <string>
#include <vector>

/*! \brief Stores images on the CPU as a list of Rgba8 values
* 
* The image class stores images as a list of Rgba8 values for each texel in the image. Images can either be created from images on disc or by passing in dimensions and a color.
* Images are completely different from Textures in that Textures are used on the GPU whereas Images are used on the CPU.
* \sa Texture
* 
*/
class Image
{
public:
	~Image() = default;
	Image(char const* imageFilePath);
	Image(IntVec2 const& size, Rgba8 const& color);

	/*! \brief Gets the path to the image on disc, relative to the game executable
	* 
	* \return A string containing the image path
	* 
	*/
	std::string const&					GetImageFilePath() const { return m_imageFilePath; }
	
	/*! \brief Gets the dimensions of the image
	* 
	* \return An IntVec2 containing the dimensions of the image 
	* 
	*/
	IntVec2								GetDimensions() const { return m_dimensions; }
	
	/*! \brief Gets the list of Rgba8 colors for all texels
	* 
	* \return A void pointer containing the texel data in the form of a list of Rgba8 colors
	* 
	*/
	const void*							GetRawData() const { return reinterpret_cast<const void*>(m_rgbaTexels.data()); }
	
	Rgba8								GetTexelColor(IntVec2 const& texelCoords);
	void								SetTexelColor(IntVec2 const& texelCoords, Rgba8 const& newColor);

private:
	//! The path to the image file on disc, relative to the game executable
	std::string							m_imageFilePath;
	//! The dimensions of the texture
	IntVec2								m_dimensions;
	//! The list of Rgba8 values for the texels in the image
	std::vector<Rgba8>					m_rgbaTexels;
};
