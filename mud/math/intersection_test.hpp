#ifndef INTERSECTION_TEXT_HPP
#define INTERSECTION_TEXT_HPP

#include "aabb.hpp"
#include "matrix.hpp"
#include "vector.hpp"

    struct Result
    {
        bool isIntersection;
    };

    struct RayCastResult
    {
        bool isIntersection;
        float distance;
    };

namespace mud
{   
    struct IntersectionResult
    {
        bool isIntersection;
    };

    struct RayCastResult
    {
        bool isIntersection;
        float distance;
    };

    namespace intersection_test
    {
        IntersectionResult pointLine();

        IntersectionResult pointCircle();

        IntersectionResult pointRect();

        IntersectionResult lineLine();

        IntersectionResult lineCircle();

        IntersectionResult lineRect();

        IntersectionResult circleCircle();

        IntersectionResult circleRect();

        IntersectionResult rectRect();

        RayCastResult rayCastPlane(const Vector3 & rayOrigin, const Vector3 & rayDirection, const Vector3 & planeCenter, const Vector3 & planeNormal);

        RayCastResult rayCastTriangle(const Vector3 & rayOrigin, const Vector3 & rayDirection, const Vector3 & p0, const Vector3 & p1, const Vector3 & p2);

        RayCastResult rayCastOBB(const Vector3 & rayOrigin, const Vector3 & rayDirection, const AABB & aabb, const Matrix4 & aabbMatrix);
    }
}

#endif