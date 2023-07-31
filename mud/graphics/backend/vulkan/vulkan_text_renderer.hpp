#ifndef VULKAN_TEXT_RENDERER_HPP
#define VULKAN_TEXT_RENDERER_HPP

#include "graphics/interface/text_renderer_base.hpp"
#include "graphics/mesh.hpp"
#include "internal/vulkan_buffer.hpp"
#include "internal/vulkan_descriptor_set.hpp"

#include <unordered_map>

namespace mud::graphics_backend::vk
{
	class VulkanSwapchain;

	class VulkanTextRenderer : public mud::TextRendererBase
	{
	public:

		VulkanTextRenderer(RenderPassOptions renderPassOptions);

		~VulkanTextRenderer();

		virtual void submit(const TextRenderCommand & command) override;

		virtual void draw(const Camera & camera) override;

	private:

		struct DrawCallData
		{
			Mesh * mesh;
			VulkanDescriptorSet * samplerDescriptorSet;
			uint32_t glyphCount;
			std::vector<TextRenderCommand> commands;
		};

		void prepareDraw(const VulkanSwapchain & swapchain);

		std::unordered_map<const Texture *, DrawCallData> m_drawCallDatas;

		VulkanDescriptorSetManager * m_descriptorSetManager;

		VkSampler m_vkTextureSampler;
		std::vector<std::vector<VulkanDescriptorSet *>> m_samplerDescriptorSets;
	};
}

#endif