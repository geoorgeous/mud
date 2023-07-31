#ifndef VULKAN_RENDER_SUB_PASS_HPP
#define VULKAN_RENDER_SUB_PASS_HPP

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/interface/render_sub_pass_base.hpp"
#include "math/matrix/matrix_4.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanRenderPass;

	class RenderSubpass : public RenderSubpassBase
	{
	public:

		RenderSubpass(const std::vector<ShaderModule *> & shaderModules);

		~RenderSubpass();

		VkPipelineLayout getPipelineLayout() const;

		const std::vector<VkPipelineShaderStageCreateInfo> & getPipelineShaderStages() const;

	private:
	
		VkPipelineLayout m_vkPipelineLayout;
		std::vector<VkPipelineShaderStageCreateInfo> m_vkPipelineShaderStages;
	};
}

#endif