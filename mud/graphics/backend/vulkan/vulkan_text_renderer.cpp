#include "vulkan_text_renderer.hpp"

#include <algorithm>

#include "graphics/camera.hpp"
#include "graphics/shader_module.hpp"
#include "internal/vulkan_debug.hpp"
#include "internal/vulkan_swapchain.hpp"
#include "platform/window.hpp"
#include "utils/file_io.hpp"
#include "utils/logger.hpp"
#include "vulkan_application_graphics_context.hpp"

namespace mud::graphics_backend::vk
{
	VulkanTextRenderer::VulkanTextRenderer(RenderPassOptions renderPassOptions)
		: TextRendererBase(renderPassOptions)
	{
		VulkanApplicationGraphicsContext * context = VulkanApplicationGraphicsContext::getInstance();
		VulkanSwapchain & swapchain = context->getMainWindow()->getGraphicsContext().getSwapchain();

		m_descriptorSetManager = new VulkanDescriptorSetManager(*swapchain.getLogicalDevice());

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

		if (!MUD__checkVulkanCall(vkCreateSampler(swapchain.getLogicalDevice()->getVulkanHandle(), &samplerInfo, nullptr, &m_vkTextureSampler), "Failed to create vkSampler for texture"))
			return;
	}

	VulkanTextRenderer::~VulkanTextRenderer()
	{
		VulkanApplicationGraphicsContext * vulkanContext = VulkanApplicationGraphicsContext::getInstance();

		VkDevice vkDevice = vulkanContext->getLogicalDevice().getVulkanHandle();

		for (auto pair : m_drawCallDatas)
			delete pair.second.mesh;

		vkDestroySampler(vkDevice, m_vkTextureSampler, nullptr);
	}

	void VulkanTextRenderer::submit(const TextRenderCommand & command)
	{
		if (command.text.empty())
			return;

		auto iter = m_drawCallDatas.find(&command.fontFace->texture);

		if (iter == m_drawCallDatas.end())
		{
			iter = m_drawCallDatas.emplace(&command.fontFace->texture, DrawCallData{}).first;
			iter->second.mesh = new Mesh;
		}

		iter->second.commands.emplace_back(command);
		iter->second.glyphCount = command.text.length();
	}

	void VulkanTextRenderer::prepareDraw(const VulkanSwapchain & swapchain)
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
			vertices.resize(iter->second.glyphCount * 4);
			indices.resize(iter->second.glyphCount * 6);
			size_t glyphIdx = 0;

			float spriteWidth = static_cast<float>(iter->first->getWidth());
			float spriteHeight = static_cast<float>(iter->first->getHeight());

			for (const TextRenderCommand & command : iter->second.commands)
			{
				Vector3 penPosition = command.position;

				for (const char & c : command.text)
				{
					if (c == '\n')
					{
						penPosition.x = command.position.x;
						penPosition.y += command.fontFace->lineHeight;
						glyphIdx++;
						continue;
					}
					else if (c == '\t')
					{
						penPosition.x += command.fontFace->spaceWidth * 4;
						glyphIdx++;
						continue;
					}

					const auto glyphIter = command.fontFace->characterGlyphs.find(c);

					if (glyphIter == command.fontFace->characterGlyphs.end())
					{
						glyphIdx++;
						continue;
					}

					const FontCharacterGlyph & glyph = glyphIter->second;

					Vector2 glyphTopLeft(penPosition.x + glyph.bearing.x, (penPosition.y - glyph.bearing.y) + command.fontFace->lineHeight);

					auto vIter = vertices.begin() + glyphIdx * 4;

					uint32_t last = static_cast<uint32_t>(vIter - vertices.begin());
					indices[glyphIdx * 6 + 0] = last + 0;
					indices[glyphIdx * 6 + 1] = last + 1;
					indices[glyphIdx * 6 + 2] = last + 2;
					indices[glyphIdx * 6 + 3] = last + 2;
					indices[glyphIdx * 6 + 4] = last + 1;
					indices[glyphIdx * 6 + 5] = last + 3;

					vIter->position.x = glyphTopLeft.x; // left top
					vIter->position.y = glyphTopLeft.y;
					vIter->colour = command.color;
					vIter->textureCoordinates.x = glyph.textureUVMin.x;
					vIter->textureCoordinates.y = glyph.textureUVMin.y;

					vIter++;
					vIter->position.x = glyphTopLeft.x; // left bottom
					vIter->position.y = glyphTopLeft.y + glyph.size.y;
					vIter->colour = command.color;
					vIter->textureCoordinates.x = glyph.textureUVMin.x;
					vIter->textureCoordinates.y = glyph.textureUVMax.y;

					vIter++;
					vIter->position.x = glyphTopLeft.x + glyph.size.x; // right top
					vIter->position.y = glyphTopLeft.y;
					vIter->colour = command.color;
					vIter->textureCoordinates.x = glyph.textureUVMax.x;
					vIter->textureCoordinates.y = glyph.textureUVMin.y;

					vIter++;
					vIter->position.x = glyphTopLeft.x + glyph.size.x; // right bottom
					vIter->position.y = glyphTopLeft.y + glyph.size.y;
					vIter->colour = command.color;
					vIter->textureCoordinates.x = glyph.textureUVMax.x;
					vIter->textureCoordinates.y = glyph.textureUVMax.y;

					penPosition.x += glyph.advance;

					glyphIdx++;
				}
			}

			iter->second.mesh->setData(vertices, indices);
			iter->second.commands.clear();
		}
	}

	void VulkanTextRenderer::draw(const Camera & camera)
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