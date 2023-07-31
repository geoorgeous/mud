#ifndef MUD_MATH_RECT
#define MUD_MATH_RECT

#include "vector.hpp"

namespace mud
{
	struct Rect
	{
		Vector2i position;
		Vector2i extents;
	};
}

#endif