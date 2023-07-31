#ifndef VULKAN_IMAGE_HPP
#define VULKAN_IMAGE_HPP

#include <vulkan/vulkan.h>
#include <vulkan/vk_mem_alloc.h>

namespace mud::graphics_backend::vk
{
	class VulkanBuffer;
	class VulkanLogicalDevice;

	class VulkanImage
	{
	public:
	
		VulkanImage(const VulkanLogicalDevice & logicalDevice, uint32_t width, uint32_t height, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags);

		~VulkanImage();

		VkImageView createView(VkImageAspectFlags vkImageAspectFlags) const;

		void transitionLayout(VkImageLayout vkImageLayoutOld, VkImageLayout vkImageLayoutNew);

		void set(const VulkanBuffer & source);

	private:

		const VulkanLogicalDevice * m_logicalDevice;
		VkImageCreateInfo m_vkCreateInfo;
		VkImage m_vkImage;
		VmaAllocation m_vmaAllocation;
	};
}

#endif