#include "vulkan_render_pass.hpp"

#include <array>

#include "internal/vulkan_debug.hpp"
#include "internal/vulkan_helpers.hpp"
#include "internal/vulkan_logical_device.hpp"
#include "internal/vulkan_swapchain.hpp"
#include "utils/logger.hpp"
#include "vulkan_application_graphics_context.hpp"

namespace mud::graphics_backend::vk
{
	RenderPass::RenderPass(const std::vector<std::vector<ShaderModule *>> & subpassesShaderModules, const RenderPassOptions & options, const std::vector<FrameBufferAttachmentInfo> & frameBufferAttachmentsInfo)
		: RenderPassBase(subpassesShaderModules, options, frameBufferAttachmentsInfo), m_callbackSwapchainOnReconstructed(this, &swapchainOnReconstructed)
	{
		log(LogLevel::Trace, fmt::format("Creating render pass with {0} subpasses...\n", m_subpasses.size()), "Vulkan");
		
		VulkanApplicationGraphicsContext * vulkanContext = VulkanApplicationGraphicsContext::getInstance();
		m_swapchain = &vulkanContext->getMainWindow()->getGraphicsContext().getSwapchain();

		std::vector<VkAttachmentDescription> vkAttachmentDescriptions;
		std::vector<VkAttachmentReference> vkAttachmentReferences;

		for (size_t idx = 0; idx < frameBufferAttachmentsInfo.size(); ++idx)
		{
			vkAttachmentDescriptions.push_back(VkAttachmentDescription{});
			vkAttachmentDescriptions.back().format = toVkImageFormat(frameBufferAttachmentsInfo[idx].imageFormat);
			vkAttachmentDescriptions.back().samples = VK_SAMPLE_COUNT_1_BIT;
			vkAttachmentDescriptions.back().loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			vkAttachmentDescriptions.back().storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			vkAttachmentDescriptions.back().stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vkAttachmentDescriptions.back().stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			vkAttachmentDescriptions.back().initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vkAttachmentDescriptions.back().finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			vkAttachmentReferences.push_back(VkAttachmentReference{});
			vkAttachmentReferences.back().attachment = idx;
			vkAttachmentReferences.back().layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		vkAttachmentDescriptions.push_back(VkAttachmentDescription{});
		vkAttachmentDescriptions.back().format = vulkanContext->getLogicalDevice().getPhysicalDevice()->findSupportedDepthFormat();
		vkAttachmentDescriptions.back().samples = VK_SAMPLE_COUNT_1_BIT;
		vkAttachmentDescriptions.back().loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vkAttachmentDescriptions.back().storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vkAttachmentDescriptions.back().stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vkAttachmentDescriptions.back().stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vkAttachmentDescriptions.back().initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vkAttachmentDescriptions.back().finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		vkAttachmentReferences.push_back(VkAttachmentReference{});
		vkAttachmentReferences.back().attachment = static_cast<uint32_t>(vkAttachmentReferences.size() - 1);
		vkAttachmentReferences.back().layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		std::vector<VkSubpassDescription> vkSubpasses(m_subpasses.size());
		std::vector<VkSubpassDependency> vkDependencies(m_subpasses.size());

		for (size_t idx = 0; idx < m_subpasses.size(); ++idx)
		{
			vkSubpasses[idx] = {};
			vkSubpasses[idx].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			vkSubpasses[idx].colorAttachmentCount = static_cast<uint32_t>(vkAttachmentReferences.size() - 1);
			vkSubpasses[idx].pColorAttachments = vkAttachmentReferences.data();
			vkSubpasses[idx].pDepthStencilAttachment = &vkAttachmentReferences.back();

			vkDependencies[idx] = {};
			vkDependencies[idx].srcSubpass = VK_SUBPASS_EXTERNAL;
			vkDependencies[idx].dstSubpass = 0;
			vkDependencies[idx].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			vkDependencies[idx].srcAccessMask = 0;
			vkDependencies[idx].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			vkDependencies[idx].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(vkAttachmentDescriptions.size());
		renderPassInfo.pAttachments = vkAttachmentDescriptions.data();
		renderPassInfo.subpassCount = vkSubpasses.size();
		renderPassInfo.pSubpasses = vkSubpasses.data();
		renderPassInfo.dependencyCount = vkDependencies.size();
		renderPassInfo.pDependencies = vkDependencies.data();

		MUD__checkVulkanCall(vkCreateRenderPass(m_swapchain->getLogicalDevice()->getVulkanHandle(), &renderPassInfo, nullptr, &m_vkRenderPass), "Failed to create render pass");

		m_vkSubpassPipelines = std::vector<VkPipeline>(subpassesShaderModules.size());
		for (size_t idx = 0; idx < m_subpasses.size(); ++idx)
		{
			VkVertexInputBindingDescription vkVertexInputBindingDescription;
			vkVertexInputBindingDescription.binding = 0;
			vkVertexInputBindingDescription.stride = 0;
			vkVertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			const std::vector<ShaderModuleInterfaceVariableDetails> & vertexShaderModuleInputVariableDetails = m_subpasses[idx]->getShaderModule(ShaderType::Vertex)->getInputVariableDetails();
			std::vector<VkVertexInputAttributeDescription> vkVertexInputAttributeDescription(vertexShaderModuleInputVariableDetails.size());

			for (size_t idxx = 0; idxx < vertexShaderModuleInputVariableDetails.size(); ++idxx)
			{
				vkVertexInputBindingDescription.stride += vertexShaderModuleInputVariableDetails[idxx].size;
				vkVertexInputAttributeDescription[idxx].binding = vkVertexInputBindingDescription.binding;
				vkVertexInputAttributeDescription[idxx].location = vertexShaderModuleInputVariableDetails[idxx].location;
				vkVertexInputAttributeDescription[idxx].format = vertexShaderModuleInputVariableDetails[idxx].format;
				vkVertexInputAttributeDescription[idxx].offset = vertexShaderModuleInputVariableDetails[idxx].offset;
			}

			VkPipelineVertexInputStateCreateInfo vertexInputState{};
			vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputState.vertexBindingDescriptionCount = 1;
			vertexInputState.pVertexBindingDescriptions = &vkVertexInputBindingDescription;
			vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vkVertexInputAttributeDescription.size());
			vertexInputState.pVertexAttributeDescriptions = vkVertexInputAttributeDescription.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = options.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = options.faceCullMode == FaceCullMode::Back ? VK_CULL_MODE_BACK_BIT : options.faceCullMode == FaceCullMode::Front ? VK_CULL_MODE_FRONT_BIT : options.faceCullMode == FaceCullMode::FrontAndBack ? VK_CULL_MODE_FRONT_BIT : VK_CULL_MODE_NONE;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f; // Optional
			rasterizer.depthBiasClamp = 0.0f;		   // Optional
			rasterizer.depthBiasSlopeFactor = 0.0f;	   // Optional

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f;			// Optional
			multisampling.pSampleMask = nullptr;			// Optional
			multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampling.alphaToOneEnable = VK_FALSE;		// Optional

			std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR };

			VkPipelineDynamicStateCreateInfo dynamicState{};
			dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicState.pDynamicStates = dynamicStates.data();

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_TRUE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f;
			colorBlending.blendConstants[1] = 0.0f;
			colorBlending.blendConstants[2] = 0.0f;
			colorBlending.blendConstants[3] = 0.0f;

			VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo{};
			depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilStateInfo.depthTestEnable = VK_TRUE;
			depthStencilStateInfo.depthWriteEnable = VK_TRUE;
			depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
			depthStencilStateInfo.minDepthBounds = 0.0f; // Optional
			depthStencilStateInfo.maxDepthBounds = 1.0f; // Optional
			depthStencilStateInfo.stencilTestEnable = VK_FALSE;
			depthStencilStateInfo.front = {}; // Optional
			depthStencilStateInfo.back = {}; // Optional

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = m_subpasses[idx]->getPipelineShaderStages().size();
			pipelineInfo.pStages = m_subpasses[idx]->getPipelineShaderStages().data();
			pipelineInfo.pVertexInputState = &vertexInputState;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicState;
			pipelineInfo.layout = m_subpasses[idx]->getPipelineLayout();
			pipelineInfo.renderPass = m_vkRenderPass;
			pipelineInfo.subpass = idx;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1;			  // Optional

			log(LogLevel::Trace, "Creating render subpass pipeline...\n", "Vulkan");

			MUD__checkVulkanCall(vkCreateGraphicsPipelines(m_swapchain->getLogicalDevice()->getVulkanHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_vkSubpassPipelines[idx]), "Failed to create graphics pipeline");
		}
		
		m_swapchain->createRenderPassFramebuffers(m_vkFramebuffers, m_vkRenderPass);
		m_vkFramebufferExtent2D = m_swapchain->getImageExtent2D();
		m_swapchain->getEventOnReconstructed().addListener(&m_callbackSwapchainOnReconstructed);
	}

	RenderPass::~RenderPass()
	{
		m_swapchain->getEventOnReconstructed().removeListener(&m_callbackSwapchainOnReconstructed);

		for (VkFramebuffer vkFramebuffer : m_vkFramebuffers)
			vkDestroyFramebuffer(m_swapchain->getLogicalDevice()->getVulkanHandle(), vkFramebuffer, nullptr);
			
		for (VkPipeline vkPipeline : m_vkSubpassPipelines)
			vkDestroyPipeline(m_swapchain->getLogicalDevice()->getVulkanHandle(), vkPipeline, nullptr);

		vkDestroyRenderPass(m_swapchain->getLogicalDevice()->getVulkanHandle(), m_vkRenderPass, nullptr);
	}

	VkRenderPass RenderPass::getVulkanHandle() const
	{
		return m_vkRenderPass;
	}

	void RenderPass::begin()
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_vkRenderPass;
		renderPassInfo.framebuffer = m_vkFramebuffers[m_swapchain->getCurrentFrameInfo().imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_vkFramebufferExtent2D;
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.1f, 0.1f, 0.1f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_swapchain->getCurrentFrameInfo().commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(renderPassInfo.renderArea.extent.width);
		viewport.height = static_cast<float>(renderPassInfo.renderArea.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_swapchain->getCurrentFrameInfo().commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = renderPassInfo.renderArea.extent;
		vkCmdSetScissor(m_swapchain->getCurrentFrameInfo().commandBuffer, 0, 1, &scissor);
		
		vkCmdBindPipeline(m_swapchain->getCurrentFrameInfo().commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkSubpassPipelines[0]);
	}

	void RenderPass::end() const
	{
		vkCmdEndRenderPass(m_swapchain->getCurrentFrameInfo().commandBuffer);
	}

	void RenderPass::swapchainOnReconstructed()
	{
		for (VkFramebuffer vkFramebuffer : m_vkFramebuffers)
			vkDestroyFramebuffer(m_swapchain->getLogicalDevice()->getVulkanHandle(), vkFramebuffer, nullptr);

		m_swapchain->createRenderPassFramebuffers(m_vkFramebuffers, m_vkRenderPass);
		m_vkFramebufferExtent2D = m_swapchain->getImageExtent2D();
	}

	void RenderPass::recreateFramebuffers()
	{
		for (VkFramebuffer vkFramebuffer : m_vkFramebuffers)
			vkDestroyFramebuffer(m_swapchain->getLogicalDevice()->getVulkanHandle(), vkFramebuffer, nullptr);

		m_swapchain->createRenderPassFramebuffers(m_vkFramebuffers, m_vkRenderPass);
		m_vkFramebufferExtent2D = m_swapchain->getImageExtent2D();
	}
}