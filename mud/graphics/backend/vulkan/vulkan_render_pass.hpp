#ifndef VULKAN_RENDER_PASS_HPP
#define VULKAN_RENDER_PASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/interface/render_pass_base.hpp"
#include "graphics/render_sub_pass.hpp"
#include "internal/vulkan_swapchain.hpp"

namespace mud::graphics_backend::vk
{
	class RenderPass : public mud::RenderPassBase
	{
	public:

		RenderPass(const std::vector<std::vector<ShaderModule *>> & subpassesShaderModules, const RenderPassOptions & options, const std::vector<FrameBufferAttachmentInfo> & frameBufferAttachmentsInfo);

		~RenderPass();

		VkRenderPass getVulkanHandle() const;

		void begin();

		void end() const;

	private:

		VulkanSwapchain::EventOnReconstructed::TListenerCallback<RenderPass> m_callbackSwapchainOnReconstructed;
		void swapchainOnReconstructed();

		void recreateFramebuffers();

		VulkanSwapchain * m_swapchain;
		VkRenderPass m_vkRenderPass;
		VkExtent2D m_vkFramebufferExtent2D;
		std::vector<VkFramebuffer> m_vkFramebuffers;
		std::vector<VkPipeline> m_vkSubpassPipelines;
	};
}

#endif