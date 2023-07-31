#ifndef VULKAN_DEBUG_HPP
#define VULKAN_DEBUG_HPP

#include <string>
#include <vulkan/vulkan.h>

#define MUD__checkVulkanCall(x, errorMessage) ::mud::graphics_backend::vk::checkVkResult(x, errorMessage)
//#define MUD__checkVulkanCall(x, errorMessage) ::mud::graphics_backend::vk::checkVkResult(x, __FILE__ " : " errorMessage)

namespace mud::graphics_backend::vk
{
	std::string getVkResultString(VkResult vkr, bool prependCode = true);

	bool checkVkResult(VkResult result, const std::string & errorMessage);
}

#endif