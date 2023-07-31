#include "vulkan_image.hpp"

#include "utils/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_logical_device.hpp"

namespace mud::graphics_backend::vk
{
	VulkanImage::VulkanImage(const VulkanLogicalDevice & logicalDevice, uint32_t width, uint32_t height, VkFormat vkFormat, VkImageTiling vkImageTiling, VkImageUsageFlags vkImageUsageFlags, VkMemoryPropertyFlags vkMemoryPropertyFlags)
		: m_logicalDevice(&logicalDevice), m_vkCreateInfo{}
	{
		m_vkCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		m_vkCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		m_vkCreateInfo.extent.width = width;
		m_vkCreateInfo.extent.height = height;
		m_vkCreateInfo.extent.depth = 1;
		m_vkCreateInfo.mipLevels = 1;
		m_vkCreateInfo.arrayLayers = 1;
		m_vkCreateInfo.format = vkFormat;
		m_vkCreateInfo.tiling = vkImageTiling;
		m_vkCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_vkCreateInfo.usage = vkImageUsageFlags;
		m_vkCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		m_vkCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo vmaAllocCreateInfo = {};
		vmaAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

		vmaCreateImage(m_logicalDevice->getAllocator(), &m_vkCreateInfo, &vmaAllocCreateInfo, &m_vkImage, &m_vmaAllocation, nullptr);
	}

	VulkanImage::~VulkanImage()
	{
		vmaDestroyImage(m_logicalDevice->getAllocator(), m_vkImage, m_vmaAllocation);
	}

	VkImageView VulkanImage::createView(VkImageAspectFlags vkImageAspectFlags) const
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_vkImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_vkCreateInfo.format;
		viewInfo.subresourceRange.aspectMask = vkImageAspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView vkNewImageView;
		MUD__checkVulkanCall(vkCreateImageView(m_logicalDevice->getVulkanHandle(), &viewInfo, nullptr, &vkNewImageView), "Failed to create vkImageView for texture");
		return vkNewImageView;
	}

	void VulkanImage::transitionLayout(VkImageLayout vkImageLayoutOld, VkImageLayout vkImageLayoutNew)
	{
		VkCommandBuffer commandBuffer = m_logicalDevice->beginSingleTimeCommands();

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = vkImageLayoutOld;
		barrier.newLayout = vkImageLayoutNew;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_vkImage;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		if (vkImageLayoutOld == VK_IMAGE_LAYOUT_UNDEFINED && vkImageLayoutNew == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		else if (vkImageLayoutOld == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && vkImageLayoutNew == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}
		else
		{
			log(LogLevel::Error, "Unsupported layout transition");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		m_logicalDevice->endSingleTimeCommands(commandBuffer);
	}

	void VulkanImage::set(const VulkanBuffer & source)
	{
		VkCommandBuffer commandBuffer = m_logicalDevice->beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { m_vkCreateInfo.extent.width, m_vkCreateInfo.extent.height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, source.getVulkanHandle(), m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		m_logicalDevice->endSingleTimeCommands(commandBuffer);
	}
}