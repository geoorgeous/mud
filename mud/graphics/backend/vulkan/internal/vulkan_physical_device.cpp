#include "vulkan_physical_device.hpp"

#include <fmt/format.h>
#include <vector>

#include "utils/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_context.hpp"
#include "vulkan_surface.hpp"

namespace mud::graphics_backend::vk
{
	bool VulkanPhysicalDevice::QueueFamilyDetails::isComplete() const
	{
		return graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value();
	}

	void VulkanPhysicalDevice::init(const VulkanContext & context, const Requirements & requirements)
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(context.getVulkanHandle(), &physicalDeviceCount, nullptr);

		if (physicalDeviceCount == 0)
		{
			log(LogLevel::Info, "Failed to find suitable GPU with Vulkan support", "Vulkan");
			return;
		}

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(context.getVulkanHandle(), &physicalDeviceCount, physicalDevices.data());

		for (const VkPhysicalDevice & physicalDevice : physicalDevices)
		{
			// Check if the physical device has the required extensions

			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

			bool oneOrMoreExtensionMissing = false;
			for (const std::string & requiredExtension : requirements.extensions)
			{
				bool extensionFound = false;

				for (const VkExtensionProperties &availableExtension : availableExtensions)
				{
					if (requiredExtension == availableExtension.extensionName)
					{
						extensionFound = true;
						break;
					}
				}

				if (!extensionFound)
				{
					oneOrMoreExtensionMissing = true;
					continue;
				}
			}

			if (oneOrMoreExtensionMissing)
				continue;

			// Check for device features

			VkPhysicalDeviceFeatures supportedFeatures;
			vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

			if (!supportedFeatures.samplerAnisotropy)
				continue;

			m_vkPhysicalDevice = physicalDevice;
		}

		if (m_vkPhysicalDevice == VK_NULL_HANDLE)
		{
			log(LogLevel::Error, "Failed to find suitable GPU", "Vulkan");
			return;
		}

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &properties);

		log(LogLevel::Info, fmt::format("Using physical device: {0}\n", properties.deviceName), "Vulkan");
	}

	VkPhysicalDevice VulkanPhysicalDevice::getVulkanHandle() const
	{
		return m_vkPhysicalDevice;
	}

	VulkanPhysicalDevice::QueueFamilyDetails VulkanPhysicalDevice::getQueueFamilyDetails(const VulkanSurface & surface) const
	{
		QueueFamilyDetails queueFamilyDetails;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		for (size_t idx = 0; idx < queueFamilies.size(); ++idx)
		{
			if (queueFamilies[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				queueFamilyDetails.graphicsFamilyIndex = idx;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice, idx, surface.getVulkanHandle(), &presentSupport);

			if (presentSupport)
				queueFamilyDetails.presentFamilyIndex = idx;

			if (queueFamilyDetails.isComplete())
				break;
		}

		return queueFamilyDetails;
	}

	VulkanPhysicalDevice::SurfaceDetails VulkanPhysicalDevice::getAvailableSurfaceDetails(const VulkanSurface & surface) const
	{
		SurfaceDetails surfaceDetails;

		VkSurfaceKHR vkSurface = surface.getVulkanHandle();

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, vkSurface, &surfaceDetails.vkSurfaceCapabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, vkSurface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			surfaceDetails.vkSurfaceFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, vkSurface, &formatCount, surfaceDetails.vkSurfaceFormats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, vkSurface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			surfaceDetails.vkPresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, vkSurface, &presentModeCount, surfaceDetails.vkPresentModes.data());
		}

		return surfaceDetails;
	}

	VkFormat VulkanPhysicalDevice::findSupportedFormat(const std::vector<VkFormat> & desiredVkFormats, VkImageTiling vkImageTiling, VkFormatFeatureFlags vkFormatFeatureFlags) const
	{
		for (VkFormat vkFormat : desiredVkFormats)
		{
			VkFormatProperties vkFormatProperties;
			vkGetPhysicalDeviceFormatProperties(m_vkPhysicalDevice, vkFormat, &vkFormatProperties);

			if ((vkImageTiling == VK_IMAGE_TILING_LINEAR && (vkFormatProperties.linearTilingFeatures & vkFormatFeatureFlags) == vkFormatFeatureFlags) ||
				(vkImageTiling == VK_IMAGE_TILING_OPTIMAL && (vkFormatProperties.optimalTilingFeatures & vkFormatFeatureFlags) == vkFormatFeatureFlags))
				return vkFormat;

		}

		log(LogLevel::Error, "Failed to find suitable vkFormat", "Vulkan");
		return VkFormat{};
	}

	VkFormat VulkanPhysicalDevice::findSupportedDepthFormat() const
	{
		return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	uint32_t VulkanPhysicalDevice::findSupportedMemory(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		log(LogLevel::Error, fmt::format("Failed to find suitable memory type\n"), "Vulkan");

		return 0;
	}
}