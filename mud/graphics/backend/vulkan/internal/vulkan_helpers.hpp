#ifndef VULKAN_HELPERS_HPP
#define VULKAN_HELPERS_HPP

#include <vulkan/vulkan.h>

#include "graphics/image_format.hpp"

namespace mud::graphics_backend::vk
{
    ImageFormat fromVkImageFormat(VkFormat vkFormat);

    VkFormat toVkImageFormat(ImageFormat imageFormat);
}

#endif