#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include "color.hpp"
#include "math/vector/vector_3.hpp"

namespace mud
{
    struct DirectionalLight
    {
        Vector3 direction;
        Color color;
    };

    struct PointLight
    {
        Vector3 position;
        Color color;
        float attenuationK;
        float attenuationL;
        float attenuationQ;
    };
}

#endif