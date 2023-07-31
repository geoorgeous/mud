#ifndef VULKAN_TEXTURE_HPP
#define VULKAN_TEXTURE_HPP

#include <vulkan/vulkan.h>

#include "graphics/interface/texture_base.hpp"
#include "internal/vulkan_image.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanLogicalDevice;

	class VulkanTexture : public mud::TextureBase
	{
	public:

		VulkanTexture();

		~VulkanTexture();

		VkImageView getVkImageView() const;

	protected:

		virtual void onSetData() override;

	private:

		const VulkanLogicalDevice * m_logicalDevice;
		VulkanImage * m_image;
		VkImageView m_vkImageView;

		virtual void freeData() override;
	};
}

#endif