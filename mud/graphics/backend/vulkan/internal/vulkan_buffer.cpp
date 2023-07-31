#include "vulkan_buffer.hpp"

#include "utils/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_logical_device.hpp"

namespace mud::graphics_backend::vk
{
	VulkanBuffer::VulkanBuffer(const VulkanLogicalDevice & logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
		: m_logicalDevice(logicalDevice)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (!MUD__checkVulkanCall(vkCreateBuffer(logicalDevice.getVulkanHandle(), &bufferInfo, nullptr, &m_vkBuffer), "Failed to create vkBuffer"))
			return;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

		VkMemoryRequirements vkMemoryRequirements;
		vkGetBufferMemoryRequirements(logicalDevice.getVulkanHandle(), m_vkBuffer, &vkMemoryRequirements);
		allocInfo.allocationSize = vkMemoryRequirements.size;
		allocInfo.memoryTypeIndex = logicalDevice.getPhysicalDevice()->findSupportedMemory(vkMemoryRequirements.memoryTypeBits, properties);

		if (!MUD__checkVulkanCall(vkAllocateMemory(logicalDevice.getVulkanHandle(), &allocInfo, nullptr, &m_vkBufferDeviceMemory), "Failed to allocate memory for vkBuffer"))
			return;

		m_size = size;

		MUD__checkVulkanCall(vkBindBufferMemory(logicalDevice.getVulkanHandle(), m_vkBuffer, m_vkBufferDeviceMemory, 0), "Failed to bind vkDeviceMemory to vkBuffer");
	}

	VulkanBuffer::~VulkanBuffer()
	{
		vkDestroyBuffer(m_logicalDevice.getVulkanHandle(), m_vkBuffer, nullptr);
		vkFreeMemory(m_logicalDevice.getVulkanHandle(), m_vkBufferDeviceMemory, nullptr);
	}

	VkBuffer VulkanBuffer::getVulkanHandle() const
	{
		return m_vkBuffer;
	}

	size_t VulkanBuffer::getSize() const
	{
		return m_size;
	}

	void VulkanBuffer::set(size_t offset, size_t size, void * data, VkMemoryMapFlags vkMemoryMapFlags)
	{
		VkDeviceSize vkOffset = static_cast<VkDeviceSize>(offset);
		VkDeviceSize vkSize = static_cast<VkDeviceSize>(size);

		void * mappedMemory;
		if (!MUD__checkVulkanCall(vkMapMemory(m_logicalDevice.getVulkanHandle(), m_vkBufferDeviceMemory, vkOffset, vkSize, vkMemoryMapFlags, &mappedMemory), "Failed to map buffer memory"))
			return;
		memcpy(mappedMemory, data, size);
		vkUnmapMemory(m_logicalDevice.getVulkanHandle(), m_vkBufferDeviceMemory);
	}

	void VulkanBuffer::copy(VulkanBuffer & dest, VkBufferCopy vkBufferCopy) const
	{
		if (vkBufferCopy.size == 0)
		{
			vkBufferCopy.dstOffset = 0;
			vkBufferCopy.srcOffset = 0;
			vkBufferCopy.size = m_size;
		}

		VkCommandBuffer commandBuffer = m_logicalDevice.beginSingleTimeCommands();

		vkCmdCopyBuffer(commandBuffer, m_vkBuffer, dest.m_vkBuffer, 1, &vkBufferCopy);

		m_logicalDevice.endSingleTimeCommands(commandBuffer);
	}
}