#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

#define MUD_ENABLE_VULKAN_DEBUG

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace mud::graphics_backend::vk
{
	class VulkanContext
	{
	public:

		~VulkanContext();

		void init(const std::vector<std::string> & layers);

		VkInstance getVulkanHandle() const;

	private:

		VkInstance m_vkInstance;

#ifdef MUD_ENABLE_VULKAN_DEBUG
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

		void initDebugUtilsMessenger();

		VkDebugUtilsMessengerEXT m_debugMessenger;
#endif
	};
}

#endif