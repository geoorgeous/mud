#ifndef VULKAN_VERTEX_INPUT_INFO_HPP
#define VULKAN_VERTEX_INPUT_INFO_HPP

#include <vulkan/vulkan.h>

namespace mud::graphics_backend::vk
{
	struct VulkanVertexInputInfo
	{
		VkVertexInputBindingDescription vkBindingDescription;
		std::vector<VkVertexInputAttributeDescription> vkAttributeDescriptions;
	};
}

#endif