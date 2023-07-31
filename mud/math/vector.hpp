#ifndef MUD_MATH_VECTOR
#define MUD_MATH_VECTOR

#include <stdint.h>

#include "vector/vector_2.hpp"
#include "vector/vector_3.hpp"
#include "vector/vector_4.hpp"
#include "vector/vector_x.hpp"

namespace mud
{
	typedef Vector<int32_t, 2> Vector2i; //!< A 2-component vector with integer components.
	typedef Vector<int32_t, 3> Vector3i; //!< A 3-component vector with integer components.
	typedef Vector<int32_t, 4> Vector4i; //!< A 4-component vector with integer components.
}

#endif