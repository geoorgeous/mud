#ifndef VULKAN_BUFFER_HPP
#define VULKAN_BUFFER_HPP

#include <vulkan/vulkan.h>

namespace mud::graphics_backend::vk
{
	class VulkanLogicalDevice;

	class VulkanBuffer
	{
	public:
	
		VulkanBuffer(const VulkanLogicalDevice & logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		VulkanBuffer(const VulkanBuffer &) = delete;
		VulkanBuffer & operator=(const VulkanBuffer &) = delete;

		~VulkanBuffer();

		VkBuffer getVulkanHandle() const;

		size_t getSize() const;

		void set(size_t offset, size_t size, void * data, VkMemoryMapFlags vkMemoryMapFlags = 0);

		void copy(VulkanBuffer & dest, VkBufferCopy vkBufferCopy = {}) const;

	private:

		const VulkanLogicalDevice & m_logicalDevice;
		VkBuffer m_vkBuffer;
		VkDeviceMemory m_vkBufferDeviceMemory;
		size_t m_size;
	};
}

#endif