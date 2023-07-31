#ifndef VULKAN_FORWARD_RENDERER_HPP
#define VULKAN_FORWARD_RENDERER_HPP

#include <array>
#include <vector>

#include <map>
#include <vulkan/vulkan.h>

#include "graphics/interface/forward_renderer_base.hpp"
#include "graphics/material.hpp"
#include "internal/vulkan_descriptor_set.hpp"
#include "math/matrix.hpp"
#include "math/vector.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanSwapchain;

	class VulkanForwardRenderer : public mud::ForwardRendererBase
	{
	public:

		VulkanForwardRenderer(RenderPassOptions renderPassOptions);

		~VulkanForwardRenderer();

		virtual void draw(const Camera & camera) override;

	private:

		struct UBO_PerObject
		{
			Matrix4 model;
			Matrix4 data;
		};

		struct GLSLPointLight
		{
			Vector4 position;
			Vector4 color;
			Vector4 attenuation;
		};

		struct GLSLDirectionalLight
		{
			Vector4 direction;
			Vector4 color;
		};	

		struct UBO_Lights
		{
			GLSLPointLight lights[256];
			uint32_t num;
		};

		VulkanDescriptorSetManager * m_descriptorSetManager;

		std::vector<UBO_PerObject> m_ubos;
		std::vector<VulkanBuffer *> m_uniformBuffers;
		std::vector<VulkanDescriptorSet *> m_uboDescriptorSets;

		UBO_Lights m_uboLights;
		std::vector<VulkanBuffer *> m_uniformBuffersLights;
		std::vector<VulkanDescriptorSet *> m_lightsDescriptorSets;

		std::vector<VkSampler> m_vkTextureSamplers;

		std::vector<std::vector<std::pair<const Material *, VulkanDescriptorSet *>>> m_samplerDescriptorSets;

		void bindTextureToDescriptorSet(VulkanDescriptorSet * descriptorSet, size_t bindIndex, const Texture * texture);

		void bindMaterialToDescriptorSets(VulkanDescriptorSet * descriptorSets, const Material * material);

		void prepareDraw(const VulkanSwapchain & swapchain);
	};
}

#endif