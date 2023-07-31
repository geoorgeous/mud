#ifndef VULKAN_SPRITE_BATCH_RENDERER_HPP
#define VULKAN_SPRITE_BATCH_RENDERER_HPP

#include <unordered_map>
#include <vector>

#include "graphics/interface/sprite_batch_renderer_base.hpp"
#include "graphics/mesh.hpp"
#include "internal/vulkan_descriptor_set.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanSwapchain;

	class VulkanSpriteBatchRenderer : public mud::SpriteBatchRendererBase
	{
	public:

		VulkanSpriteBatchRenderer(RenderPassOptions renderPassOptions);

		~VulkanSpriteBatchRenderer();

		virtual void submit(const SpriteRenderCommand & command) override;

		virtual void draw(const Camera & camera) override;

	private:

		struct DrawCallData
		{
			Mesh * mesh;
			std::vector<SpriteRenderCommand> commands;
			VulkanDescriptorSet * samplerDescriptorSet;
		};

		std::unordered_map<const Texture *, DrawCallData> m_drawCallDatas;

		VulkanDescriptorSetManager * m_descriptorSetManager;

		VkSampler m_vkTextureSampler;
		std::vector<std::vector<VulkanDescriptorSet *>> m_samplerDescriptorSets;

		void prepareDraw(const VulkanSwapchain & swapchain);
	};
}

#endif