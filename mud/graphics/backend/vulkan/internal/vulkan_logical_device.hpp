#ifndef VULKAN_LOGICAL_DEVICE_HPP
#define VULKAN_LOGICAL_DEVICE_HPP

#include <string>
#include <vector>
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "vulkan_physical_device.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanContext;

	class VulkanLogicalDevice
	{
	public:

		~VulkanLogicalDevice();

		void init(const VulkanContext & context, const VulkanPhysicalDevice & physicalDevice, const VulkanPhysicalDevice::QueueFamilyDetails & queueFamilyDetails, const std::vector<std::string> & enabledExtensions, const std::vector<std::string> & enabledLayers);

		const VulkanPhysicalDevice * getPhysicalDevice() const;

		VkDevice getVulkanHandle() const;

		VkQueue getGraphicsQueue() const;

		VkQueue getPresentQueue() const;

		const VmaAllocator & getAllocator() const;

		const VkDescriptorPool & getDescriptorPool() const;

		const VkCommandPool & getCommandPool() const;

		VkCommandBuffer beginSingleTimeCommands() const;

		void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

		bool createDescriptorSets(std::vector<VkDescriptorSet> & descriptorSets, const VkDescriptorSetLayout & descriptorSetLayout);

	private:

		const VulkanPhysicalDevice * m_physicalDevice;

		VkDevice m_vkDevice;
		VkQueue m_vkQueueGraphics;
		VkQueue m_vkQueuePresent;

		VmaAllocator m_vmaAllocator;
		VkDescriptorPool m_vkDescriptorPool;
		VkCommandPool m_vkCommandPool;
	};
}

#endif