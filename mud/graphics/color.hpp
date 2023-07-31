#ifndef COLOR_HPP
#define COLOR_HPP

#include "math/vector.hpp"

namespace mud
{
	struct Color
	{
		static const Color aqua;
		static const Color aquaMarine;
		static const Color beige;
		static const Color black;
		static const Color blue;
		static const Color brown;
		static const Color cornflowerBlue;
		static const Color crimson;
		static const Color cyan;
		static const Color forestGreen;
		static const Color gold;
		static const Color green;
		static const Color grey;
		static const Color hotPink;
		static const Color indigo;
		static const Color ivory;
		static const Color khaki;
		static const Color lavendar;
		static const Color lime;
		static const Color magenta;
		static const Color maroon;
		static const Color midnightBlue;
		static const Color navy;
		static const Color olive;
		static const Color orange;
		static const Color orchid;
		static const Color pink;
		static const Color plum;
		static const Color purple;
		static const Color red;
		static const Color royalBlue;
		static const Color saddleBrown;
		static const Color salmon;
		static const Color silver;
		static const Color skyBlue;
		static const Color slateGray;
		static const Color tan;
		static const Color teal;
		static const Color transparent;
		static const Color turquoise;
		static const Color violet;
		static const Color white;
		static const Color yellow;

		Color();

		Color(float r, float g, float b, float a = 1.0f);

		Color(const Vector3 & vector, float a = 1.0f);

		Color(const Vector4 & vector);

		Color(uint32_t hex);

		Color(const Color & rgb, float a);

		Vector4 toVector() const;

		operator Vector4() const;

		float r, g, b, a;
	};
}

#endif