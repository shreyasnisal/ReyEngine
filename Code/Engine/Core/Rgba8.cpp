#include "Engine/Core/Rgba8.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

/*! \brief Construct an Rgba8 color using the provided red, green, blue and alpha components
* 
* \param redByte The amount of red in the color represented as a value from 0 to 255
* \param greenByte The amount of green in the color represented as a value from 0 to 255
* \param blueByte The amount of blue in the color represented as a value from 0 to 255
* \param alphaByte The opacity of the color represented as a value from 0 to 255
*/
Rgba8::Rgba8(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
	: r(redByte)
	, g(greenByte)
	, b(blueByte)
	, a(alphaByte)
{

}

/*! \brief Constructs the Rgba8 color from a string
* 
* Fatal error if the number of comma-separated values in the string are not 3 or 4. For 3 comma-separated values, an alpha of 255 is used. If the number of comma-separated components is correct but any of the components cannot be parsed to an integer, that component is set to a value of 0.
* \param text The string to be parsed for the color
* 
*/
void Rgba8::SetFromText(char const* text)
{
	Strings splitStrings;
	int numSplitStrings = SplitStringOnDelimiter(splitStrings, text, ',', true);

	if (numSplitStrings == 3)
	{
		r = static_cast<unsigned char>(GetClamped(static_cast<float>(atoi(splitStrings[0].c_str())), 0.f, 255.f));
		g = static_cast<unsigned char>(GetClamped(static_cast<float>(atoi(splitStrings[1].c_str())), 0.f, 255.f));
		b = static_cast<unsigned char>(GetClamped(static_cast<float>(atoi(splitStrings[2].c_str())), 0.f, 255.f));
		a = 255;
	}
	else if (numSplitStrings == 4)
	{
		r = static_cast<unsigned char>(GetClamped(static_cast<float>(atoi(splitStrings[0].c_str())), 0.f, 255.f));
		g = static_cast<unsigned char>(GetClamped(static_cast<float>(atoi(splitStrings[1].c_str())), 0.f, 255.f));
		b = static_cast<unsigned char>(GetClamped(static_cast<float>(atoi(splitStrings[2].c_str())), 0.f, 255.f));
		a = static_cast<unsigned char>(GetClamped(static_cast<float>(atoi(splitStrings[3].c_str())), 0.f, 255.f));
	}
	else
	{
		ERROR_AND_DIE("Incorrect number of literals in Rgba8 string!");
	}
}

/*! \brief Multiplies the R, G and B values of this color with the color provided, using the scaling factor provided
* 
* Multiplying with black darkens a color, whereas multiplying with white lightens it. Note that this multiplication does not affect the color's opacity.
* \param colorToAdd The Rgba8 color that this color should be multiplied with
* \param uniformScale A float representing the scaling factor to be used when multiplying the colors
* 
*/
void Rgba8::MultiplyRGBScaled(Rgba8 const& colorToAdd, float uniformScale)
{
	float redFloat = float(r) / 255.f;
	float colorToAddRedFloat = float(colorToAdd.r) / 255.f;
	r = (unsigned char)RoundDownToInt(RangeMapClamped(redFloat * colorToAddRedFloat * uniformScale, 0.f, 1.f, 0.f, 255.f));

	float greenFloat = float(g) / 255.f;
	float colorToAddGreenFloat = float(colorToAdd.g) / 255.f;
	g = (unsigned char)RoundDownToInt(RangeMapClamped(greenFloat * colorToAddGreenFloat * uniformScale, 0.f, 1.f, 0.f, 255.f));

	float blueFloat = float(b) / 255.f;
	float colorToAddBlueFloat = float(colorToAdd.b) / 255.f;
	b = (unsigned char)RoundDownToInt(RangeMapClamped(blueFloat * colorToAddBlueFloat * uniformScale, 0.f, 1.f, 0.f, 255.f));
}

/*! \brief Returns normalized values for the color components
* 
* Each component of the color is converted from the range [0, 255] to [0.f, 1.f] and the floats are populated in the array passed as an argument.
* \param out_colorAsFloats The array in which the float values for the color components are populated
* 
*/
// #TODO Use MathUtils::NormalizeByte instead of division by 255
void Rgba8::GetAsFloats(float* out_colorAsFloats) const
{
	out_colorAsFloats[0] = NormalizeByte(r);
	out_colorAsFloats[1] = NormalizeByte(g);
	out_colorAsFloats[2] = NormalizeByte(b);
	out_colorAsFloats[3] = NormalizeByte(a);
}

bool Rgba8::CompareRgba(Rgba8 const& colorToCompare) const
{
	return (r == colorToCompare.r && g == colorToCompare.g && b == colorToCompare.b && a == colorToCompare.a);
}

bool Rgba8::CompareRgb(Rgba8 const& colorToCompare) const
{
	return (r == colorToCompare.r && g == colorToCompare.g && b == colorToCompare.b);
}
