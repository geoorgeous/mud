#include "vulkan_shader_module.hpp"

#include <spirv_cross/spirv_glsl.hpp>

#include "graphics/backend/spirv/spirv.hpp"
#include "internal/vulkan_debug.hpp"
#include "internal/vulkan_descriptor_set.hpp"
#include "vulkan_application_graphics_context.hpp"
#include "utils/logger.hpp"

namespace mud::graphics_backend::vk
{
	VkDescriptorType spvToVk(SpvReflectDescriptorType spvReflectDescriptorType)
	{
		switch (spvReflectDescriptorType)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;//VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
			return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		}

		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

	VkFormat spvToVk(SpvReflectFormat spvReflectFormat)
    {
		switch (spvReflectFormat)
		{
		case SPV_REFLECT_FORMAT_UNDEFINED:
			return VK_FORMAT_UNDEFINED;
		case SPV_REFLECT_FORMAT_R32_UINT:
			return VK_FORMAT_R32_UINT;
		case SPV_REFLECT_FORMAT_R32_SINT:
			return VK_FORMAT_R32_SINT;
		case SPV_REFLECT_FORMAT_R32_SFLOAT:
			return VK_FORMAT_R32_SFLOAT;
		case SPV_REFLECT_FORMAT_R32G32_UINT:
			return VK_FORMAT_R32G32_UINT;
		case SPV_REFLECT_FORMAT_R32G32_SINT:
			return VK_FORMAT_R32G32_SINT;
		case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
			return VK_FORMAT_R32G32_SFLOAT;
		case SPV_REFLECT_FORMAT_R32G32B32_UINT:
			return VK_FORMAT_R32G32B32_UINT;
		case SPV_REFLECT_FORMAT_R32G32B32_SINT:
			return VK_FORMAT_R32G32B32_SINT;
		case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
			return VK_FORMAT_R32G32B32A32_UINT;
		case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
			return VK_FORMAT_R32G32B32A32_SINT;
		case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SPV_REFLECT_FORMAT_R64_UINT:
			return VK_FORMAT_R64_UINT;
		case SPV_REFLECT_FORMAT_R64_SINT:
			return VK_FORMAT_R64_SINT;
		case SPV_REFLECT_FORMAT_R64_SFLOAT:
			return VK_FORMAT_R64_SFLOAT;
		case SPV_REFLECT_FORMAT_R64G64_UINT:
			return VK_FORMAT_R64G64_UINT;
		case SPV_REFLECT_FORMAT_R64G64_SINT:
			return VK_FORMAT_R64G64_SINT;
		case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
			return VK_FORMAT_R64G64_SFLOAT;
		case SPV_REFLECT_FORMAT_R64G64B64_UINT:
			return VK_FORMAT_R64G64B64_UINT;
		case SPV_REFLECT_FORMAT_R64G64B64_SINT:
			return VK_FORMAT_R64G64B64_SINT;
		case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
			return VK_FORMAT_R64G64B64_SFLOAT;
		case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
			return VK_FORMAT_R64G64B64A64_UINT;
		case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
			return VK_FORMAT_R64G64B64A64_SINT;
		case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
			return VK_FORMAT_R64G64B64A64_SFLOAT;
		}
		// unhandled SpvReflectFormat enum value
		return VK_FORMAT_UNDEFINED;
	}

	VkShaderStageFlagBits VulkanShaderModule::getVkShaderStageFlagBit(ShaderType shaderType)
	{
		switch (shaderType)
		{
		case ShaderType::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;

		case ShaderType::Fragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}

	VulkanShaderModule::VulkanShaderModule(ShaderType type)
		: ShaderModuleBase(type), m_vkPipelineShaderStageCreateInfo{}
	{
		m_vkPipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_vkPipelineShaderStageCreateInfo.pName = "main";
		m_vkPipelineShaderStageCreateInfo.stage = getVkShaderStageFlagBit();
	}

	VulkanShaderModule::~VulkanShaderModule()
	{
		for (VulkanDescriptorSetLayout * layout : m_descriptorSetLayouts)
			delete layout;

		vkDestroyShaderModule(VulkanApplicationGraphicsContext::getInstance()->getLogicalDevice().getVulkanHandle(), m_vkPipelineShaderStageCreateInfo.module, nullptr);
	}

	bool VulkanShaderModule::compileSource()
	{
		if (!ShaderModuleBase::compileSource())
			return false;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_spirvByteCode.size() * sizeof(uint32_t);
		createInfo.pCode = m_spirvByteCode.data();

		if (!MUD__checkVulkanCall(vkCreateShaderModule(VulkanApplicationGraphicsContext::getInstance()->getLogicalDevice().getVulkanHandle(), &createInfo, nullptr, &m_vkPipelineShaderStageCreateInfo.module), "Failed to create shader module"))
			return false;

		SpvReflectShaderModule module;
		SpvReflectResult result = spvReflectCreateShaderModule(m_spirvByteCode.size() * sizeof(uint32_t), static_cast<void *>(m_spirvByteCode.data()), &module);

		uint32_t count = 0;

		result = spvReflectEnumerateInputVariables(&module, &count, NULL);
		if (count > 0)
		{
			std::vector<SpvReflectInterfaceVariable *> inputVariables(count);
			result = spvReflectEnumerateInputVariables(&module, &count, inputVariables.data());

			m_inputVariableDetails.resize(count);
			for (size_t idx = 0; idx < count; ++idx)
			{
				const uint32_t location = inputVariables[idx]->location;
				m_inputVariableDetails[location].binding = 0;
				m_inputVariableDetails[location].location = location;
				m_inputVariableDetails[location].format = spvToVk(inputVariables[idx]->format);
				m_inputVariableDetails[location].size = inputVariables[idx]->type_description ? graphics_backend::spirv::sizeOfType(*inputVariables[idx]->type_description) : 0;
			}
			
			for (size_t idx = 0; idx < count; ++idx)
			{
				m_inputVariableDetails[idx].offset = 0;
				for (size_t idxx = 0; idxx < count; ++idxx)
					if (m_inputVariableDetails[idxx].location < m_inputVariableDetails[idx].location)
						m_inputVariableDetails[idx].offset += m_inputVariableDetails[idxx].size;
			}
		}

		result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
		if (count > 0)
		{
			std::vector<SpvReflectDescriptorSet *> descriptorSets(count);
			result = spvReflectEnumerateDescriptorSets(&module, &count, descriptorSets.data());

			for (const SpvReflectDescriptorSet * set : descriptorSets)
			{
				const SpvReflectDescriptorSet & descriptorSet = *set;

				std::vector<VulkanDescriptorSetLayout::Binding> bindings;

				for (uint32_t idx = 0; idx < descriptorSet.binding_count; ++idx)
				{
					const SpvReflectDescriptorBinding & binding = *descriptorSet.bindings[idx];

					bindings.push_back(VulkanDescriptorSetLayout::Binding{});
					bindings.back().count = binding.count;
					bindings.back().type = spvToVk(binding.descriptor_type);
					bindings.back().stages = m_vkPipelineShaderStageCreateInfo.stage;
				}

				VulkanDescriptorSetLayout * newDescriptorSetLayout = new VulkanDescriptorSetLayout(VulkanApplicationGraphicsContext::getInstance()->getLogicalDevice(), bindings);
				m_descriptorSetLayouts.push_back(newDescriptorSetLayout);
			}
		}

		spvReflectDestroyShaderModule(&module);

		return true;
	}

	const VkShaderModule & VulkanShaderModule::getVulkanModule() const
	{
		return m_vkPipelineShaderStageCreateInfo.module;
	}

	const VkPipelineShaderStageCreateInfo & VulkanShaderModule::getPipelineShaderStageCreateInfo() const
	{
		return m_vkPipelineShaderStageCreateInfo;
	}

	const std::vector<VulkanDescriptorSetLayout *> & VulkanShaderModule::getDescriptorSetLayouts() const
	{
		return m_descriptorSetLayouts;
	}

	const std::vector<ShaderModuleInterfaceVariableDetails> & VulkanShaderModule::getInputVariableDetails() const
	{
		return m_inputVariableDetails;
	}

	const VkShaderStageFlagBits VulkanShaderModule::getVkShaderStageFlagBit() const
	{
		return getVkShaderStageFlagBit(m_type);
	}
}