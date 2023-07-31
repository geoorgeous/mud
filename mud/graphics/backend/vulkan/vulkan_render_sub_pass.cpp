#include "vulkan_render_sub_pass.hpp"

#include "graphics/backend/vulkan/internal/vulkan_debug.hpp"
#include "graphics/backend/vulkan/internal/vulkan_descriptor_set.hpp"
#include "graphics/backend/vulkan/vulkan_vertex_input_info.hpp"
#include "graphics/render_pass.hpp"
#include "utils/logger.hpp"
#include "vulkan_application_graphics_context.hpp"

namespace mud::graphics_backend::vk
{
    RenderSubpass::RenderSubpass(const std::vector<ShaderModule *> & shaderModules)
        : RenderSubpassBase(shaderModules)
    {
		log(LogLevel::Trace, "Creating render subpass...\n", "Vulkan");

		std::vector<VkPushConstantRange> pushConstants;
		for (const ShaderModule * shaderModule : shaderModules)
		{
			const auto & blocksDetails = shaderModule->getPushConstantBlocksDetails();
			for (const auto & details : blocksDetails)
				pushConstants.push_back(VkPushConstantRange{static_cast<VkShaderStageFlags>(shaderModule->getVkShaderStageFlagBit()), static_cast<uint32_t>(details.second.offset), static_cast<uint32_t>(details.second.size)});
		}

		std::vector<VkDescriptorSetLayout> setLayouts;

		for (const VulkanDescriptorSetLayout * layout : getShaderModule(ShaderType::Vertex)->getDescriptorSetLayouts())
			setLayouts.push_back(layout->getVulkanHandle());
		for (const VulkanDescriptorSetLayout * layout : getShaderModule(ShaderType::Fragment)->getDescriptorSetLayouts())
			setLayouts.push_back(layout->getVulkanHandle());

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
		pipelineLayoutInfo.pSetLayouts = setLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
		pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

		MUD__checkVulkanCall(vkCreatePipelineLayout(VulkanApplicationGraphicsContext::getInstance()->getLogicalDevice().getVulkanHandle(), &pipelineLayoutInfo, nullptr, &m_vkPipelineLayout), "Failed to create pipeline layout for render subpass");
    
		m_vkPipelineShaderStages = {
			getShaderModule(ShaderType::Vertex)->getPipelineShaderStageCreateInfo(),
			getShaderModule(ShaderType::Fragment)->getPipelineShaderStageCreateInfo()
		};
	}

    RenderSubpass::~RenderSubpass()
    {
		vkDestroyPipelineLayout(VulkanApplicationGraphicsContext::getInstance()->getLogicalDevice().getVulkanHandle(), m_vkPipelineLayout, nullptr);
    }

	VkPipelineLayout RenderSubpass::getPipelineLayout() const
	{
		return m_vkPipelineLayout;
	}

	const std::vector<VkPipelineShaderStageCreateInfo> & RenderSubpass::getPipelineShaderStages() const
	{
		return m_vkPipelineShaderStages;
	}
}