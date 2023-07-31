#include "intersection_test.hpp"

#include <cmath>

#include "utils/logger.hpp"

namespace mud::intersection_test
{
    RayCastResult rayCastPlane(const Vector3 & rayOrigin, const Vector3 & rayDirection, const Vector3 & planeCenter, const Vector3 & planeNormal)
    {
        RayCastResult result;
        result.isIntersection = false;
        result.distance = 0;

        float denom = planeNormal.dot(rayDirection);
        if (std::fabs(denom) <= 0.000001f) // your favorite epsilon
            return result;

        float t = (planeCenter - rayOrigin).dot(planeNormal) / denom;
        if (t <= 0.000001f)
            return result;

        result.isIntersection = true;
        result.distance = t;
        return result;
    }

    RayCastResult rayCastTriangle(const Vector3 & rayOrigin, const Vector3 & rayDirection, const Vector3 & p0, const Vector3 & p1, const Vector3 & p2)
    {
        RayCastResult result;
        result.isIntersection = false;
        result.distance = 0;

        // Moller Trumbore ray-triangle intersection

        Vector3 p0p1 = p1 - p0;
        Vector3 p0p2 = p2 - p0;
        Vector3 pvec = rayDirection.cross(p0p2);
        float det = p0p1.dot(pvec);

        if (std::fabs(det) < 0.000001f)
            return result;

        float invDet = 1 / det;

        Vector3 tvec = rayOrigin - p0;
        float u = tvec.dot(pvec) * invDet;
        if (u < 0 || u > 1)
            return result;

        Vector3 qvec = tvec.cross(p0p1);
        float v = rayDirection.dot(qvec) * invDet;
        if (v < 0 || u + v > 1)
            return result;
        
        float t = p0p2.dot(qvec) * invDet;
        if (t < 0)
            return result;

        result.distance = t;
        result.isIntersection = true;
        return result;
    }

    RayCastResult rayCastOBB(const Vector3 & rayOrigin, const Vector3 & rayDirection, const AABB & aabb, const Matrix4 & aabbMatrix)
    {
        RayCastResult result;
        result.isIntersection = false;
        result.distance = 0;

        const Vector3 aabbMatrixScale(aabbMatrix[0].x, aabbMatrix[1].y, aabbMatrix[2].z);
        const AABB scaledAABB(aabb.min * aabbMatrixScale, aabb.max * aabbMatrixScale);

        const float threshold = 0.000001;
        float tMin = 0;
        float tMax = 100000.0f;

        const Vector3 pos(aabbMatrix[3].x, aabbMatrix[3].y, aabbMatrix[3].z);
        const Vector3 delta = pos - rayOrigin;

        // Test intersection with the 2 planes perpendicular to the OBB's X axis
        {
            const Vector3 xaxis(aabbMatrix[0].x, aabbMatrix[0].y, aabbMatrix[0].z);
            const float e = xaxis.dot(delta);
            const float f = rayDirection.dot(xaxis);

            if (std::fabs(f) > threshold) { // Standard case

                float t1 = (e + scaledAABB.min.x) / f;
                float t2 = (e + scaledAABB.max.x) / f;

                if (t1 > t2) {
                    const float w = t1;
                    t1 = t2;
                    t2 = w;
                }

                if (t2 < tMax)
                    tMax = t2;
                if (t1 > tMin)
                    tMin = t1;
                if (tMax < tMin)
                    return result;

            } else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
                if (-e + scaledAABB.min.x > 0.0 || -e + scaledAABB.max.x < 0.0)
                    return result;
            }
        }

        {
            const Vector3 yaxis(aabbMatrix[1].x, aabbMatrix[1].y, aabbMatrix[1].z);
            const float e = yaxis.dot(delta);
            const float f = rayDirection.dot(yaxis);

            if (std::fabs(f) > threshold) {

                float t1 = (e + scaledAABB.min.y) / f;
                float t2 = (e + scaledAABB.max.y) / f;

                if (t1 > t2) {
                    const float w = t1;
                    t1 = t2;
                    t2 = w;
                }

                if (t2 < tMax)
                    tMax = t2;
                if (t1 > tMin)
                    tMin = t1;
                if (tMin > tMax)
                    return result;

            } else {
                if (-e + scaledAABB.min.y > 0.0 || -e + scaledAABB.max.y < 0.0)
                    return result;
            }
        }

        {
            const Vector3 yaxis(aabbMatrix[2].x, aabbMatrix[2].y, aabbMatrix[2].z);
            const float e = yaxis.dot(delta);
            const float f = rayDirection.dot(yaxis);

            if (std::fabs(f) > threshold) {

                float t1 = (e + scaledAABB.min.z) / f;
                float t2 = (e + scaledAABB.max.z) / f;

                if (t1 > t2) {
                    const float w = t1;
                    t1 = t2;
                    t2 = w;
                }

                if (t2 < tMax)
                    tMax = t2;
                if (t1 > tMin)
                    tMin = t1;
                if (tMin > tMax)
                    return result;

            } else {
                if (-e + scaledAABB.min.z > 0.0 || -e + scaledAABB.max.z < 0.0)
                    return result;
            }
        }

        result.isIntersection = true;
        result.distance = tMin;
        return result;
    }
}