#pragma once

/*! \brief Represents a color containing RGBA values as bytes
* 
* Each of the values is stored as a byte (as opposed to conventions where all values are stored as floats or the alpha value is stored as a float).
* 
*/
struct Rgba8
{
public:
	//! The amount of red component in the color
	unsigned char r = 255;
	//! The amount of green component in the color
	unsigned char g = 255;
	//! The amount of blue component in the color
	unsigned char b = 255;
	//! The alphe (opacity) of the color
	unsigned char a = 255;

public:
	//! Default destructor for an Rgba9
	~Rgba8() = default;
	//! Default constructor for an Rgba8
	Rgba8() = default;
	explicit Rgba8(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte);
	
	void SetFromText(char const* text);
	void MultiplyRGBScaled(Rgba8 const& colorToAdd, float uniformScale);
	void GetAsFloats(float* out_colorAsFloats) const;
	bool CompareRgba(Rgba8 const& colorToCompare) const;
	bool CompareRgb(Rgba8 const& colorToCompare) const;

	static const Rgba8 BLACK;
	static const Rgba8 WHITE;
	static const Rgba8 RED;
	static const Rgba8 LIME;
	static const Rgba8 BLUE;
	static const Rgba8 YELLOW;
	static const Rgba8 CYAN;
	//! Alternate name for Rgba8::CYAN
	static const Rgba8 AQUA;
	static const Rgba8 MAGENTA;
	//! Alternate name for Rgba8::MAGENTA
	static const Rgba8 FUCHSIA;
	static const Rgba8 SILVER;
	static const Rgba8 GRAY;
	static const Rgba8 MAROON;
	static const Rgba8 OLIVE;
	static const Rgba8 GREEN;
	static const Rgba8 PURPLE;
	static const Rgba8 TEAL;
	static const Rgba8 NAVY;
	static const Rgba8 ORANGE;
	static const Rgba8 ROYAL_BLUE;
	static const Rgba8 STEEL_BLUE;
	static const Rgba8 DODGER_BLUE;
	static const Rgba8 DEEP_SKY_BLUE;
	static const Rgba8 BROWN;
	static const Rgba8 TRANSPARENT_WHITE;
	static const Rgba8 TRANSPARENT_BLACK;
};

inline const Rgba8 Rgba8::BLACK{0, 0, 0, 255};
inline const Rgba8 Rgba8::WHITE{255, 255, 255, 255};
inline const Rgba8 Rgba8::RED{255, 0, 0, 255};
inline const Rgba8 Rgba8::LIME{0, 255, 0, 255};
inline const Rgba8 Rgba8::BLUE{0, 0, 255, 255};
inline const Rgba8 Rgba8::YELLOW{255, 255, 0, 255};
inline const Rgba8 Rgba8::CYAN{0, 255, 255, 255};
inline const Rgba8 Rgba8::AQUA{0, 255, 255, 255};
inline const Rgba8 Rgba8::MAGENTA{255, 0, 255, 255};
inline const Rgba8 Rgba8::FUCHSIA{255, 0, 255, 255};
inline const Rgba8 Rgba8::SILVER{192, 192, 192, 255};
inline const Rgba8 Rgba8::GRAY{128, 128, 128, 255};
inline const Rgba8 Rgba8::MAROON{128, 0, 0, 255};
inline const Rgba8 Rgba8::OLIVE{128, 128, 0, 255};
inline const Rgba8 Rgba8::GREEN{0, 128, 0, 255};
inline const Rgba8 Rgba8::PURPLE{128, 0, 128, 255};
inline const Rgba8 Rgba8::TEAL{0, 128, 128, 255};
inline const Rgba8 Rgba8::NAVY{0, 0, 128, 255};
inline const Rgba8 Rgba8::ORANGE{255, 165, 0, 255};
inline const Rgba8 Rgba8::ROYAL_BLUE{65, 105, 225, 255};
inline const Rgba8 Rgba8::STEEL_BLUE{70, 130, 180, 255};
inline const Rgba8 Rgba8::DODGER_BLUE{30, 144, 255, 255};
inline const Rgba8 Rgba8::DEEP_SKY_BLUE{0, 191, 255, 255};
inline const Rgba8 Rgba8::BROWN{165, 42, 42, 255};
inline const Rgba8 Rgba8::TRANSPARENT_WHITE{255, 255, 255, 0};
inline const Rgba8 Rgba8::TRANSPARENT_BLACK{0, 0, 0, 0};
