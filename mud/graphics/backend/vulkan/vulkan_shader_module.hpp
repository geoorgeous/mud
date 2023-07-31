#ifndef VULKAN_SHADER_MODULE_HPP
#define VULKAN_SHADER_MODULE_HPP

#include <vector>
#include <vulkan/vulkan.h>

#include "graphics/interface/shader_module_base.hpp"
#include "internal/vulkan_logical_device.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanDescriptorSetLayout;

	struct ShaderModuleInterfaceVariableDetails
	{
		uint32_t binding;
		uint32_t location;
		VkFormat format;
		uint32_t offset;
		uint32_t size;
	};

	class VulkanShaderModule : public ShaderModuleBase
	{
	public:

		static VkShaderStageFlagBits getVkShaderStageFlagBit(ShaderType shaderType);

		VulkanShaderModule(ShaderType type);

		~VulkanShaderModule();

		virtual bool compileSource() override;

		const VkShaderModule & getVulkanModule() const;

		const VkPipelineShaderStageCreateInfo & getPipelineShaderStageCreateInfo() const;

		const std::vector<VulkanDescriptorSetLayout *> & getDescriptorSetLayouts() const;

		const std::vector<ShaderModuleInterfaceVariableDetails> & getInputVariableDetails() const;

		const VkShaderStageFlagBits getVkShaderStageFlagBit() const;

	private:

		VkPipelineShaderStageCreateInfo m_vkPipelineShaderStageCreateInfo;

		std::vector<VulkanDescriptorSetLayout *> m_descriptorSetLayouts;

		std::vector<ShaderModuleInterfaceVariableDetails> m_inputVariableDetails;
	};
}

#endif