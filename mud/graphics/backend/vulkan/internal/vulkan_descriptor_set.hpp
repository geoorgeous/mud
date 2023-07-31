#ifndef VULKAN_DESCRIPTOR_SET_HPP
#define VULKAN_DESCRIPTOR_SET_HPP

#include <vector>
#include <vulkan/vulkan.h>

#include "graphics/shader_module.hpp"
#include "math/matrix.hpp"
#include "vulkan_debug.hpp"
#include "utils/logger.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanLogicalDevice;
	class VulkanRenderPass;

	class VulkanDescriptorSetLayout
	{
	public:

		struct Binding
		{
			VkDescriptorType type;
			uint32_t count;
			VkShaderStageFlags stages;
		};

		VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout &) = delete;

		VulkanDescriptorSetLayout(const VulkanLogicalDevice & logicalDevice, std::vector<Binding> bindings);

		~VulkanDescriptorSetLayout();

		const std::vector<Binding> & getBindings() const;

		VkDescriptorSetLayout getVulkanHandle() const;

		VkWriteDescriptorSet getVkWriteDescriptorSetForBinding(uint32_t binding) const;

	private:

		std::vector<Binding> m_bindings;

		const VulkanLogicalDevice & m_logicalDevice;

		VkDescriptorSetLayout m_vkDescriptorSetLayout;

		std::vector<VkWriteDescriptorSet> m_vkWriteDescriptorSets;
	};

	class VulkanDescriptorSet
	{
	public:

		VulkanDescriptorSet(const VulkanDescriptorSetLayout & layout);

		VulkanDescriptorSet(const VulkanDescriptorSet &) = delete;

		VulkanDescriptorSet(VulkanDescriptorSet && other);

		const VulkanDescriptorSetLayout & getLayout();

		void setVulkanHandle(VkDescriptorSet vkDescriptorSet);

		VkDescriptorSet getVulkanHandle() const;

		VkDescriptorImageInfo * getVkWriteImageInfoForBinding(uint32_t binding);

		VkDescriptorBufferInfo * getVkWriteBufferInfoForBinding(uint32_t binding);

	private:

		const VulkanDescriptorSetLayout & m_layout;
		VkDescriptorSet m_vkDescriptorSet;
		std::vector<VkDescriptorImageInfo> m_vkWriteImageInfo;
		std::vector<VkDescriptorBufferInfo> m_vkWriteBufferInfo;
	};

	// class VulkanDescriptorSet
	// {
	// private:

	// 	VulkanDescriptorSetLayout m_layout;
	// };

	class VulkanDescriptorSetManager
	{
	public:

		VulkanDescriptorSetManager(const VulkanLogicalDevice & logicalDevice);

		VulkanDescriptorSet * allocate(const VulkanDescriptorSetLayout & layout);

		void update(VulkanDescriptorSet * set, uint32_t binding, VkDescriptorBufferInfo vkDescriptorBufferInfo);

		void update(VulkanDescriptorSet * set, uint32_t binding, VkDescriptorImageInfo vkDescriptorImageInfo);

		void bind(VulkanDescriptorSet * set);

		void bind(VulkanDescriptorSet * set, uint32_t dynamicOffset);

		void free(VulkanDescriptorSet *& set);

		void doAllocates();

		void doUpdates();

		void doBinds(VkCommandBuffer vkCommandBuffer, VkPipelineLayout vkPipelineLayout);

		void doFrees();

	private:

		const VulkanLogicalDevice & m_logicalDevice;

		std::vector<VulkanDescriptorSet *> m_toAllocate;
		std::vector<VulkanDescriptorSet *> m_toUpdate;
		std::vector<VkWriteDescriptorSet> m_pendingUpdateWrites;
		std::vector<VulkanDescriptorSet *> m_toBind;
		std::vector<uint32_t> m_bindDynamicOffsets;
		std::vector<VulkanDescriptorSet **> m_toFree;
	};
}

#endif