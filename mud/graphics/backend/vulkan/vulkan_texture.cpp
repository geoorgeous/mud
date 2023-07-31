#include "vulkan_texture.hpp"

#include "utils/logger.hpp"
#include "internal/vulkan_buffer.hpp"
#include "internal/vulkan_debug.hpp"
#include "internal/vulkan_helpers.hpp"
#include "internal/vulkan_logical_device.hpp"
#include "vulkan_application_graphics_context.hpp"

namespace mud::graphics_backend::vk
{
	VulkanTexture::VulkanTexture()
		: TextureBase(), m_logicalDevice(nullptr), m_image(nullptr), m_vkImageView(VK_NULL_HANDLE)
	{}

	VulkanTexture::~VulkanTexture()
	{
		vkDestroyImageView(m_logicalDevice->getVulkanHandle(), m_vkImageView, nullptr);
		delete m_image;
	}

	VkImageView VulkanTexture::getVkImageView() const
	{
		return m_vkImageView;
	}

	void VulkanTexture::onSetData()
	{
		if (m_sizeBytes <= 0)
			return;

		if (m_logicalDevice == nullptr)
			m_logicalDevice = &VulkanApplicationGraphicsContext::getInstance()->getLogicalDevice();

		VulkanBuffer stagingBuffer(*m_logicalDevice, m_sizeBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.set(0, m_sizeBytes, m_data);

		delete m_image;

		m_image = new VulkanImage(*m_logicalDevice, m_width, m_height, toVkImageFormat(m_imageFormat), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_image->transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_image->set(stagingBuffer);
		m_image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		if (m_vkImageView != VK_NULL_HANDLE)
			vkDestroyImageView(m_logicalDevice->getVulkanHandle(), m_vkImageView, nullptr);

		m_vkImageView = m_image->createView(VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanTexture::freeData()
	{
		TextureBase::freeData();

		vkDestroyImageView(m_logicalDevice->getVulkanHandle(), m_vkImageView, nullptr);
		m_vkImageView = VK_NULL_HANDLE;
		delete m_image;
	}
}