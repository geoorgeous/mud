#ifndef IMAGE_FORMAT_HPP
#define IMAGE_FORMAT_HPP

namespace mud
{
    enum class ImageFormat
    {
        Undefined,
        R8_SRGB,
        R8_UNORM,
        R8G8_SRGB,
        R8G8_UNORM,
        R8G8B8_SRGB,
        R8G8B8_UNORM,
        R8G8B8A8_SRGB,
        R8G8B8A8_UNORM,
        B8G8R8_SRGB,
        B8G8R8_UNORM,
        B8G8R8A8_SRGB,
        B8G8R8A8_UNORM,
        R16G16B16_SFLOAT,
        R16G16B16A16_SFLOAT,
    };
}

#endif