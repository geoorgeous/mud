#include "vulkan_deferred_renderer.hpp"

#include "graphics/backend/spirv/spirv.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader_module.hpp"
#include "internal/vulkan_debug.hpp"
#include "internal/vulkan_swapchain.hpp"
#include "math/constants.hpp"
#include "math/matrix/transformations.hpp"
#include "platform/window.hpp"
#include "utils/asset_manager.hpp"
#include "utils/file_io.hpp"
#include "utils/logger.hpp"
#include "vulkan_application_graphics_context.hpp"

#define MAX_OBJECTS 32768

namespace mud::graphics_backend::vk
{
    VulkanDeferredRenderer::VulkanDeferredRenderer(RenderPassOptions renderPassOptions)
		: DeferredRendererBase(renderPassOptions)
	{
		VulkanApplicationGraphicsContext * vulkanContext = VulkanApplicationGraphicsContext::getInstance();
		VulkanSwapchain & swapchain = vulkanContext->getMainWindow()->getGraphicsContext().getSwapchain();

		m_descriptorSetManager = new VulkanDescriptorSetManager(*swapchain.getLogicalDevice());

		m_uniformBuffers.resize(swapchain.getImages().size());
		m_uniformBuffersLights.resize(swapchain.getImages().size());

		const ShaderModule * vertexShaderModule = m_renderPass->getSubpasses()[0]->getShaderModule(ShaderType::Vertex);
		const ShaderModule * fragmentShaderModule = m_renderPass->getSubpasses()[0]->getShaderModule(ShaderType::Fragment);

		for (size_t idx = 0; idx < swapchain.getImages().size(); idx++)
		{
			// ubos

			m_uniformBuffers[idx] = new VulkanBuffer(*swapchain.getLogicalDevice(), sizeof(UBO_PerObject) * MAX_OBJECTS, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			VulkanDescriptorSet * newDescriptorSet = m_descriptorSetManager->allocate(*vertexShaderModule->getDescriptorSetLayouts()[0]);

			VkDescriptorBufferInfo vkDescriptorBufferInfo{};
			vkDescriptorBufferInfo.buffer = m_uniformBuffers[idx]->getVulkanHandle();
			vkDescriptorBufferInfo.offset = 0;
			vkDescriptorBufferInfo.range = sizeof(UBO_PerObject);

			m_descriptorSetManager->update(newDescriptorSet, 0, vkDescriptorBufferInfo);

			m_uboDescriptorSets.push_back(newDescriptorSet);

			// lights
			
			m_uniformBuffersLights[idx] = new VulkanBuffer(*swapchain.getLogicalDevice(), sizeof(UBO_Lights), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			newDescriptorSet = m_descriptorSetManager->allocate(*fragmentShaderModule->getDescriptorSetLayouts()[0]);

			vkDescriptorBufferInfo.buffer = m_uniformBuffersLights[idx]->getVulkanHandle();
			vkDescriptorBufferInfo.offset = 0;
			vkDescriptorBufferInfo.range = sizeof(UBO_Lights);

			m_descriptorSetManager->update(newDescriptorSet, 0, vkDescriptorBufferInfo);

			m_lightsDescriptorSets.push_back(newDescriptorSet);
		}

		m_descriptorSetManager->doAllocates();
		m_descriptorSetManager->doUpdates();

		m_samplerDescriptorSets.resize(swapchain.getImages().size());

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(swapchain.getLogicalDevice()->getPhysicalDevice()->getVulkanHandle(), &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		for (size_t idx = 0; idx < 4; ++idx)
		{
			m_vkTextureSamplers.emplace_back();
			if (!MUD__checkVulkanCall(vkCreateSampler(swapchain.getLogicalDevice()->getVulkanHandle(), &samplerInfo, nullptr, &m_vkTextureSamplers[idx]), "Failed to create vkSampler"))
				return;
		}
	}

	VulkanDeferredRenderer::~VulkanDeferredRenderer()
	{
		VulkanApplicationGraphicsContext * vulkanContext = VulkanApplicationGraphicsContext::getInstance();

		VkDevice vkDevice = vulkanContext->getLogicalDevice().getVulkanHandle();

		delete m_descriptorSetManager;

		for (VulkanBuffer * uniformBuffer : m_uniformBuffers)
			delete uniformBuffer;

		for (VulkanBuffer * uniformBuffer : m_uniformBuffersLights)
			delete uniformBuffer;

		for (VkSampler vkSampler : m_vkTextureSamplers)
			vkDestroySampler(vkDevice, vkSampler, nullptr);
	}
}