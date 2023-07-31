#include "vulkan_helpers.hpp"

namespace mud::graphics_backend::vk
{
    ImageFormat fromVkImageFormat(VkFormat vkFormat)
    {
        switch (vkFormat)
		{
            case VK_FORMAT_UNDEFINED:           return ImageFormat::Undefined;
			case VK_FORMAT_R8_SRGB:             return ImageFormat::R8_SRGB;
			case VK_FORMAT_R8_UNORM:            return ImageFormat::R8_UNORM;
			case VK_FORMAT_R8G8_SRGB:           return ImageFormat::R8G8_SRGB;
			case VK_FORMAT_R8G8_UNORM:          return ImageFormat::R8G8_UNORM;
			case VK_FORMAT_R8G8B8_SRGB:         return ImageFormat::R8G8B8_SRGB;
			case VK_FORMAT_R8G8B8_UNORM:        return ImageFormat::R8G8B8_UNORM;
			case VK_FORMAT_R8G8B8A8_SRGB:       return ImageFormat::R8G8B8A8_SRGB;
			case VK_FORMAT_R8G8B8A8_UNORM:      return ImageFormat::R8G8B8A8_UNORM;
			case VK_FORMAT_B8G8R8_SRGB:         return ImageFormat::B8G8R8_SRGB;
			case VK_FORMAT_B8G8R8_UNORM:        return ImageFormat::B8G8R8_UNORM;
			case VK_FORMAT_B8G8R8A8_SRGB:       return ImageFormat::B8G8R8A8_SRGB;
			case VK_FORMAT_B8G8R8A8_UNORM:      return ImageFormat::B8G8R8A8_UNORM;
			case VK_FORMAT_R16G16B16_SFLOAT:    return ImageFormat::R16G16B16_SFLOAT;
			case VK_FORMAT_R16G16B16A16_SFLOAT: return ImageFormat::R16G16B16A16_SFLOAT;
		}
        return ImageFormat::Undefined;
    }

    VkFormat toVkImageFormat(ImageFormat imageFormat)
    {
        switch (imageFormat)
		{
            case ImageFormat::Undefined:            return VK_FORMAT_UNDEFINED;
			case ImageFormat::R8_SRGB:              return VK_FORMAT_R8_SRGB;
			case ImageFormat::R8_UNORM:             return VK_FORMAT_R8_UNORM;
			case ImageFormat::R8G8_SRGB:            return VK_FORMAT_R8G8_SRGB;
			case ImageFormat::R8G8_UNORM:           return VK_FORMAT_R8G8_UNORM;
			case ImageFormat::R8G8B8_SRGB:          return VK_FORMAT_R8G8B8_SRGB;
			case ImageFormat::R8G8B8_UNORM:         return VK_FORMAT_R8G8B8_UNORM;
			case ImageFormat::R8G8B8A8_SRGB:        return VK_FORMAT_R8G8B8A8_SRGB;
			case ImageFormat::R8G8B8A8_UNORM:       return VK_FORMAT_R8G8B8A8_UNORM;
			case ImageFormat::B8G8R8_SRGB:          return VK_FORMAT_B8G8R8_SRGB;
			case ImageFormat::B8G8R8_UNORM:         return VK_FORMAT_B8G8R8_UNORM;
			case ImageFormat::B8G8R8A8_SRGB:        return VK_FORMAT_B8G8R8A8_SRGB;
			case ImageFormat::B8G8R8A8_UNORM:       return VK_FORMAT_B8G8R8A8_UNORM;
			case ImageFormat::R16G16B16_SFLOAT:     return VK_FORMAT_R16G16B16_SFLOAT;
			case ImageFormat::R16G16B16A16_SFLOAT:  return VK_FORMAT_R16G16B16A16_SFLOAT;
		}
        return VK_FORMAT_UNDEFINED;
    }
}