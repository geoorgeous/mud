#include "vulkan_forward_renderer.hpp"

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
	VulkanForwardRenderer::VulkanForwardRenderer(RenderPassOptions renderPassOptions)
		: ForwardRendererBase(renderPassOptions)
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

	VulkanForwardRenderer::~VulkanForwardRenderer()
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

	void VulkanForwardRenderer::bindTextureToDescriptorSet(VulkanDescriptorSet * descriptorSet, size_t bindIndex, const Texture * texture)
	{
		VkDescriptorImageInfo vkDescriptorImageInfo{};

		if (texture == nullptr)
		{
			log(LogLevel::Error, "Failed to bind texture to descriptor set: Texture is null\n");
			return;
		}
			
		vkDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vkDescriptorImageInfo.imageView = texture->getVkImageView();
		vkDescriptorImageInfo.sampler = m_vkTextureSamplers[bindIndex];
		m_descriptorSetManager->update(descriptorSet, bindIndex, vkDescriptorImageInfo);
	}

	void VulkanForwardRenderer::bindMaterialToDescriptorSets(VulkanDescriptorSet * descriptorSet, const Material * material)
	{
		bindTextureToDescriptorSet(descriptorSet, 0, material->diffuseMap->get());
		bindTextureToDescriptorSet(descriptorSet, 1, material->normalMap->get());
		bindTextureToDescriptorSet(descriptorSet, 2, material->metalnessMap->get());
		bindTextureToDescriptorSet(descriptorSet, 3, material->roughnessMap->get());
	}

	void VulkanForwardRenderer::prepareDraw(const VulkanSwapchain & swapchain)
	{
		const VulkanSwapchain::FrameInfo &frameInfo = swapchain.getCurrentFrameInfo();
		auto & frameSamplerDescriptorSets = m_samplerDescriptorSets[frameInfo.index];

		for (auto & pair : frameSamplerDescriptorSets)
			m_descriptorSetManager->free(pair.second);

		m_descriptorSetManager->doFrees();
		frameSamplerDescriptorSets.clear();
		
		std::sort(m_commands.begin(), m_commands.end());

		m_ubos.resize(m_commands.size());
		const Material * lastMaterial = nullptr;
		
		const ShaderModule * fragmentShaderModule = m_renderPass->getSubpasses()[0]->getShaderModule(ShaderType::Fragment);

		for (size_t idx = 0; idx < m_commands.size(); ++idx)
		{
			RenderCommand & command = m_commands[idx];
			
			m_ubos[idx].model = m_commands[idx].transform;
			m_ubos[idx].data[0] = m_commands[idx].material->baseColor;

			if (command.material != lastMaterial)
			{
				VulkanDescriptorSet * descriptorSet = m_descriptorSetManager->allocate(*fragmentShaderModule->getDescriptorSetLayouts()[1]);
				bindMaterialToDescriptorSets(descriptorSet, command.material);
				frameSamplerDescriptorSets.emplace_back(command.material, descriptorSet);
				lastMaterial = command.material;
			}
		}

		m_uboLights.num = static_cast<uint32_t>(m_lights.size());
		for (size_t idx = 0; idx < m_lights.size(); ++idx)
		{
			m_uboLights.lights[idx].position = m_lights[idx].position;
			m_uboLights.lights[idx].color = m_lights[idx].color;
			m_uboLights.lights[idx].attenuation.x = m_lights[idx].attenuationK;
			m_uboLights.lights[idx].attenuation.y = m_lights[idx].attenuationL;
			m_uboLights.lights[idx].attenuation.z = m_lights[idx].attenuationQ;
			m_uboLights.lights[idx].attenuation.w = 0.0f; // padding
		}
		m_uniformBuffersLights[frameInfo.index]->set(0, sizeof(UBO_Lights), &m_uboLights);

		m_descriptorSetManager->doAllocates();
		m_descriptorSetManager->doUpdates();
	}

	void VulkanForwardRenderer::draw(const Camera & camera)
	{
		if (m_commands.empty())
			return;
			
		m_renderPass->begin();

		VulkanApplicationGraphicsContext * context = VulkanApplicationGraphicsContext::getInstance();
		VulkanSwapchain & swapchain = context->getMainWindow()->getGraphicsContext().getSwapchain();
		const VulkanSwapchain::FrameInfo & frameInfo = swapchain.getCurrentFrameInfo();

		prepareDraw(swapchain);

		// Push constants

		ShaderModulePushConstantBlock * vertexShaderPushConstants = m_renderPass->getSubpasses()[0]->getPushConstantBlock(ShaderType::Vertex, "VertexShaderPushConstants");
		Matrix4 & projectionMatrix = *vertexShaderPushConstants->getMember<Matrix4>("projectionMatrix");
		projectionMatrix = camera.getProjectionMatrix();
		projectionMatrix[1][1] *= -1;
		Matrix4 & viewMatrix = *vertexShaderPushConstants->getMember<Matrix4>("viewMatrix");
		viewMatrix = camera.getViewMatrix();

		vkCmdPushConstants(
			frameInfo.commandBuffer,
			m_renderPass->getSubpasses()[0]->getPipelineLayout(),
			VK_SHADER_STAGE_VERTEX_BIT,
			static_cast<uint32_t>(vertexShaderPushConstants->getOffset()),
			static_cast<uint32_t>(vertexShaderPushConstants->getSize()),
			vertexShaderPushConstants->getData());

		ShaderModulePushConstantBlock * fragmentShaderPushConstants = m_renderPass->getSubpasses()[0]->getPushConstantBlock(ShaderType::Fragment, "FragmentShaderPushConstants");
		GLSLDirectionalLight & directionalLight = *fragmentShaderPushConstants->getMember<GLSLDirectionalLight>("directionalLight");
		directionalLight.direction = m_directionalLight.direction;
		directionalLight.color = m_directionalLight.color.toVector();

		vkCmdPushConstants(
			frameInfo.commandBuffer,
			m_renderPass->getSubpasses()[0]->getPipelineLayout(),
			VK_SHADER_STAGE_FRAGMENT_BIT,
			static_cast<uint32_t>(fragmentShaderPushConstants->getOffset()),
			static_cast<uint32_t>(fragmentShaderPushConstants->getSize()),
			fragmentShaderPushConstants->getData());

		// Fill uniform buffer

		m_uniformBuffers[frameInfo.index]->set(0, sizeof(UBO_PerObject) * m_ubos.size(), m_ubos.data());

		const Material * lastMaterial = nullptr;

		size_t commandIdx = 0;
		for (; commandIdx < m_commands.size(); ++commandIdx)
		{
			const RenderCommand &command = m_commands[commandIdx];

			// Vertex buffer

			const Mesh *mesh = command.mesh;

			VkBuffer vertexBuffers[] = {mesh->getVertexBuffer()->getVulkanHandle()};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

			// Descriptor sets

			uint32_t dynamicOffset = commandIdx * sizeof(UBO_PerObject);
			m_descriptorSetManager->bind(m_uboDescriptorSets[frameInfo.index], dynamicOffset);
			m_descriptorSetManager->bind(m_lightsDescriptorSets[frameInfo.index], 0);

			if (command.material != lastMaterial)
			{
				for (const auto & pair : m_samplerDescriptorSets[frameInfo.index])
					if (pair.first == command.material)
					{
						m_descriptorSetManager->bind(pair.second);
						break;
					}

				lastMaterial = command.material;
			}

			m_descriptorSetManager->doBinds(frameInfo.commandBuffer, m_renderPass->getSubpasses()[0]->getPipelineLayout());

			// DRAW!

			if (mesh->getIndices().empty())
			{
				vkCmdDraw(frameInfo.commandBuffer, mesh->getVertices().size(), 1, 0, 0);
			}
			else
			{
				vkCmdBindIndexBuffer(frameInfo.commandBuffer, mesh->getIndexBuffer()->getVulkanHandle(), 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(frameInfo.commandBuffer, static_cast<uint32_t>(mesh->getIndices().size()), 1, 0, 0, 0);
			}
		}

		m_commands.erase(m_commands.begin(), m_commands.begin() + commandIdx);
		m_ubos.erase(m_ubos.begin(), m_ubos.begin() + commandIdx);
		m_lights.clear();

		m_renderPass->end();
	}
}