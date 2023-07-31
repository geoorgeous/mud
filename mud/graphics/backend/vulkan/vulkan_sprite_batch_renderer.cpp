#include "vulkan_sprite_batch_renderer.hpp"

#include <algorithm>

#include "graphics/camera.hpp"
#include "internal/vulkan_buffer.hpp"
#include "internal/vulkan_debug.hpp"
#include "internal/vulkan_swapchain.hpp"
#include "utils/file_io.hpp"
#include "vulkan_application_graphics_context.hpp"

namespace mud::graphics_backend::vk
{
	VulkanSpriteBatchRenderer::VulkanSpriteBatchRenderer(RenderPassOptions renderPassOptions)
		: SpriteBatchRendererBase(renderPassOptions)
	{
		VulkanApplicationGraphicsContext * vulkanContext = VulkanApplicationGraphicsContext::getInstance();
		VulkanSwapchain & swapchain = vulkanContext->getMainWindow()->getGraphicsContext().getSwapchain();

		m_descriptorSetManager = new VulkanDescriptorSetManager(*swapchain.getLogicalDevice());

		m_samplerDescriptorSets.resize(swapchain.getImages().size());

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(swapchain.getLogicalDevice()->getPhysicalDevice()->getVulkanHandle(), &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
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

		if (!MUD__checkVulkanCall(vkCreateSampler(swapchain.getLogicalDevice()->getVulkanHandle(), &samplerInfo, nullptr, &m_vkTextureSampler), "Failed to create vkSampler for texture"))
			return;
	}

	VulkanSpriteBatchRenderer::~VulkanSpriteBatchRenderer()
	{
		VulkanApplicationGraphicsContext * vulkanContext = VulkanApplicationGraphicsContext::getInstance();

		VkDevice vkDevice = vulkanContext->getLogicalDevice().getVulkanHandle();

		for (auto pair : m_drawCallDatas)
			delete pair.second.mesh;

		delete m_descriptorSetManager;

		vkDestroySampler(vkDevice, m_vkTextureSampler, nullptr);
	}

	void VulkanSpriteBatchRenderer::submit(const SpriteRenderCommand & command)
	{
		auto iter = m_drawCallDatas.find(command.texture);

		if (iter == m_drawCallDatas.end())
		{
			iter = m_drawCallDatas.emplace(command.texture, DrawCallData{}).first;
			iter->second.mesh = new Mesh;
		}

		iter->second.commands.emplace_back(command);
	}

	void VulkanSpriteBatchRenderer::prepareDraw(const VulkanSwapchain & swapchain)
	{
		const VulkanSwapchain::FrameInfo & frameInfo = swapchain.getCurrentFrameInfo();

		auto & samplerDescriptorSets = m_samplerDescriptorSets[frameInfo.index];

		for (auto & set : samplerDescriptorSets)
			m_descriptorSetManager->free(set);
		m_descriptorSetManager->doFrees();

		samplerDescriptorSets.resize(m_drawCallDatas.size());

		const ShaderModule * fragmentShaderModule = m_renderPass->getSubpasses()[0]->getShaderModule(ShaderType::Fragment);

		size_t descriptorSetIdx = 0;
		for (auto iter = m_drawCallDatas.begin(); iter != m_drawCallDatas.end(); iter++, ++descriptorSetIdx)
		{
			samplerDescriptorSets[descriptorSetIdx] = m_descriptorSetManager->allocate(*fragmentShaderModule->getDescriptorSetLayouts()[0]);
			iter->second.samplerDescriptorSet = samplerDescriptorSets[descriptorSetIdx];

			VkDescriptorImageInfo vkDescriptorImageInfo{};
			vkDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			vkDescriptorImageInfo.imageView = iter->first->getVkImageView();
			vkDescriptorImageInfo.sampler = m_vkTextureSampler;

			m_descriptorSetManager->update(iter->second.samplerDescriptorSet, 0, vkDescriptorImageInfo);
		}

		m_descriptorSetManager->doAllocates();
		m_descriptorSetManager->doUpdates();

		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;

		for (auto iter = m_drawCallDatas.begin(); iter != m_drawCallDatas.end(); iter++)
		{
			vertices.resize(iter->second.commands.size() * 4);
			indices.resize(iter->second.commands.size() * 6);
			size_t spriteIdx = 0;

			for (const SpriteRenderCommand & command : iter->second.commands)
			{
				auto vIter = vertices.begin() + spriteIdx * 4;

				uint32_t last = static_cast<uint32_t>(vIter - vertices.begin());
				indices[spriteIdx * 6 + 0] = last + 0;
				indices[spriteIdx * 6 + 1] = last + 1;
				indices[spriteIdx * 6 + 2] = last + 2;
				indices[spriteIdx * 6 + 3] = last + 2;
				indices[spriteIdx * 6 + 4] = last + 1;
				indices[spriteIdx * 6 + 5] = last + 3;

				vIter->position.x = 0; // left top
				vIter->position.y = 0;
				vIter->colour = command.color;
				vIter->textureCoordinates.x = 0;
				vIter->textureCoordinates.y = 0;
				vIter->position = Vector3(command.transform * Vector4(vIter->position, 1.0f));

				vIter++;
				vIter->position.x = 0; // left bottom
				vIter->position.y = 1.0f;
				vIter->colour = command.color;
				vIter->textureCoordinates.x = 0;
				vIter->textureCoordinates.y = 1;
				vIter->position = Vector3(command.transform * Vector4(vIter->position, 1.0f));

				vIter++;
				vIter->position.x = 1.0f; // right top
				vIter->position.y = 0;
				vIter->colour = command.color;
				vIter->textureCoordinates.x = 1;
				vIter->textureCoordinates.y = 0;
				vIter->position = Vector3(command.transform * Vector4(vIter->position, 1.0f));

				vIter++;
				vIter->position.x = 1.0f; // right bottom
				vIter->position.y = 1.0f;
				vIter->colour = command.color;
				vIter->textureCoordinates.x = 1;
				vIter->textureCoordinates.y = 1;
				vIter->position = Vector3(command.transform * Vector4(vIter->position, 1.0f));

				spriteIdx++;
			}

			iter->second.mesh->setData(vertices, indices);

			iter->second.commands.clear();
		}
	}

	void VulkanSpriteBatchRenderer::draw(const Camera & camera)
	{
		for (auto iter = m_drawCallDatas.begin(); iter != m_drawCallDatas.end();)
		{
			if (iter->second.commands.empty())
			{
				iter = m_drawCallDatas.erase(iter);
				continue;
			}
			iter++;
		}

		if (m_drawCallDatas.empty())
			return;

		m_renderPass->begin();

		VulkanApplicationGraphicsContext * context = VulkanApplicationGraphicsContext::getInstance();
		const VulkanSwapchain & swapchain = context->getMainWindow()->getGraphicsContext().getSwapchain();
		const VulkanSwapchain::FrameInfo & frameInfo = swapchain.getCurrentFrameInfo();

		prepareDraw(swapchain);

		// Push constants
		
		ShaderModulePushConstantBlock * vertexShaderPushConstants = m_renderPass->getSubpasses()[0]->getPushConstantBlock(ShaderType::Vertex, "PushConstants");
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

		for (auto pair : m_drawCallDatas)
		{
			// Vertex buffer

			VkBuffer vertexBuffers[] = { pair.second.mesh->getVertexBuffer()->getVulkanHandle() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(frameInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

			// Descriptor sets

			m_descriptorSetManager->bind(pair.second.samplerDescriptorSet);
			m_descriptorSetManager->doBinds(frameInfo.commandBuffer, m_renderPass->getSubpasses()[0]->getPipelineLayout());

			// DRAW!

			vkCmdBindIndexBuffer(frameInfo.commandBuffer, pair.second.mesh->getIndexBuffer()->getVulkanHandle(), 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(frameInfo.commandBuffer, static_cast<uint32_t>(pair.second.mesh->getIndices().size()), 1, 0, 0, 0);
		}

		m_renderPass->end();
	}
}