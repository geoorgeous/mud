#ifndef MUD_AABB_HPP
#define MUD_AABB_HPP

#include "vector/vector_3.hpp"

namespace mud
{
    struct AABB
    {
        Vector3 min;
        Vector3 max;

        AABB() = default;

        AABB(const Vector3 & min, const Vector3 & max);
    };  
}

#endif