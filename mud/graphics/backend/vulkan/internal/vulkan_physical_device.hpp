#ifndef VULKAN_PHYSICAL_DEVICE_HPP
#define VULKAN_PHYSICAL_DEVICE_HPP

#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace mud::graphics_backend::vk
{
	class VulkanContext;
	class VulkanSurface;

	class VulkanPhysicalDevice
	{
	public:

		struct Requirements
		{
			std::vector<std::string> extensions;
		};

		struct QueueFamilyDetails
		{
			std::optional<uint32_t> graphicsFamilyIndex;
			std::optional<uint32_t> presentFamilyIndex;

			bool isComplete() const;
		};

		struct SurfaceDetails
		{
			VkSurfaceCapabilitiesKHR vkSurfaceCapabilities;
			std::vector<VkSurfaceFormatKHR> vkSurfaceFormats;
			std::vector<VkPresentModeKHR> vkPresentModes;
		};

		void init(const VulkanContext & context, const Requirements & requirements);

		VkPhysicalDevice getVulkanHandle() const;

		QueueFamilyDetails getQueueFamilyDetails(const VulkanSurface & surface) const;

		SurfaceDetails getAvailableSurfaceDetails(const VulkanSurface & surface) const;

		VkFormat findSupportedFormat(const std::vector<VkFormat> & desiredVkFormats, VkImageTiling vkImageTiling, VkFormatFeatureFlags vkFormatFeatureFlags) const;

		VkFormat findSupportedDepthFormat() const;

		uint32_t findSupportedMemory(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	private:

		VkPhysicalDevice m_vkPhysicalDevice;
	};
}

#endif