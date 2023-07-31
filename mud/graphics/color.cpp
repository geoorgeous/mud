#include "color.hpp"

namespace mud
{
	// from: https://www.w3schools.com/colors/colors_names.asp
	//									(0xRRGGBBAA)
	const Color Color::aqua				(0x00FFFFFF);
	const Color Color::aquaMarine		(0x7FFFD4FF);
	const Color Color::beige			(0xF5F5DCFF);
	const Color Color::black			(0x000000FF);
	const Color Color::blue				(0x0000FFFF);
	const Color Color::brown			(0xA52A2AFF);
	const Color Color::cornflowerBlue	(0x6495EDFF);
	const Color Color::crimson			(0xDC143CFF);
	const Color Color::cyan				(0x00FFFFFF);
	const Color Color::forestGreen		(0x228B22FF);
	const Color Color::gold				(0xFFD700FF);
	const Color Color::green			(0x008000FF);
	const Color Color::grey				(0x808080FF);
	const Color Color::hotPink			(0xFF69B4FF);
	const Color Color::indigo			(0x4B0082FF);
	const Color Color::ivory			(0xFFFFF0FF);
	const Color Color::khaki			(0xF0E68CFF);
	const Color Color::lavendar			(0xE6E6FAFF);
	const Color Color::lime				(0x00FF00FF);
	const Color Color::magenta			(0xFF00FFFF);
	const Color Color::maroon			(0x800000FF);
	const Color Color::midnightBlue		(0x191970FF);
	const Color Color::navy				(0x000080FF);
	const Color Color::olive			(0x808000FF);
	const Color Color::orange			(0xFFA500FF);
	const Color Color::orchid			(0xDA70D6FF);
	const Color Color::pink				(0xFFC0CBFF);
	const Color Color::plum				(0xDDA0DDFF);
	const Color Color::purple			(0x800080FF);
	const Color Color::red				(0xFF0000FF);
	const Color Color::royalBlue		(0x4169E1FF);
	const Color Color::saddleBrown		(0x8B4513FF);
	const Color Color::salmon			(0xFA8072FF);
	const Color Color::silver			(0xC0C0C0FF);
	const Color Color::skyBlue			(0x87CEEBFF);
	const Color Color::slateGray		(0x708090FF);
	const Color Color::tan				(0xD2B48CFF);
	const Color Color::teal				(0x008080FF);
	const Color Color::transparent		(0x00000000);
	const Color Color::turquoise		(0x40E0D0FF);
	const Color Color::violet			(0xEE82EEFF);
	const Color Color::white			(0xFFFFFFFF);
	const Color Color::yellow			(0xFFFF00FF);

	Color::Color()
		: r(0.0f), g(r), b(r), a(1.0f)
	{ }

	Color::Color(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{ }

	Color::Color(const Vector3 & vector, float a)
		: Color(vector.x, vector.y, vector.z, a)
	{ }

	Color::Color(const Vector4 & vector)
		: Color(vector.x, vector.y, vector.z, vector.w)
	{ }

	Color::Color(uint32_t hex)
	{
		r = ((hex >> 24) & 0xFF) / 255.0;
		g = ((hex >> 16) & 0xFF) / 255.0;
		b = ((hex >> 8) & 0xFF) / 255.0;
		a = ((hex) & 0xFF) / 255.0;
	}

	Color::Color(const Color & rgb, float a)
		: r(rgb.r), g(rgb.g), b(rgb.b), a(a)
	{ }

	Vector4 Color::toVector() const
	{
		return Vector4(r, g, b, a);
	}

	Color::operator Vector4() const
	{
		return toVector();
	}
}