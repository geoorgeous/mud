#include "aabb.hpp"

namespace mud
{
    AABB::AABB(const Vector3 & min, const Vector3 & max)
        : min(min), max(max)
    { }
}