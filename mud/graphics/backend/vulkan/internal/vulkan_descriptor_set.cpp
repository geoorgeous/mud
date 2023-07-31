#include "vulkan_descriptor_set.hpp"

namespace mud::graphics_backend::vk
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanLogicalDevice & logicalDevice, std::vector<Binding> bindings)
		: m_logicalDevice(logicalDevice), m_bindings(bindings)
	{
		std::vector<VkDescriptorSetLayoutBinding> vkLayoutBindings(bindings.size());

		for (size_t idx = 0; idx < m_bindings.size(); ++idx)
		{
			vkLayoutBindings[idx].binding = static_cast<uint32_t>(idx);
			vkLayoutBindings[idx].descriptorType = m_bindings[idx].type;
			vkLayoutBindings[idx].descriptorCount = m_bindings[idx].count;
			vkLayoutBindings[idx].stageFlags = m_bindings[idx].stages;
			vkLayoutBindings[idx].pImmutableSamplers = nullptr;
		}

		VkDescriptorSetLayoutCreateInfo vkLayoutCreateInfo{};
		vkLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		vkLayoutCreateInfo.bindingCount = static_cast<uint32_t>(vkLayoutBindings.size());
		vkLayoutCreateInfo.pBindings = vkLayoutBindings.data();
		vkLayoutCreateInfo.flags = 0;

		MUD__checkVulkanCall(vkCreateDescriptorSetLayout(m_logicalDevice.getVulkanHandle(), &vkLayoutCreateInfo, nullptr, &m_vkDescriptorSetLayout), "failed to create descriptor set layout!");

		m_vkWriteDescriptorSets.resize(bindings.size());
		for (size_t idx = 0; idx < bindings.size(); ++idx)
		{
			m_vkWriteDescriptorSets[idx] = VkWriteDescriptorSet{};
			m_vkWriteDescriptorSets[idx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			m_vkWriteDescriptorSets[idx].dstSet = VK_NULL_HANDLE;
			m_vkWriteDescriptorSets[idx].dstBinding = idx;
			m_vkWriteDescriptorSets[idx].descriptorType = bindings[idx].type;
			m_vkWriteDescriptorSets[idx].dstArrayElement = 0;
			m_vkWriteDescriptorSets[idx].descriptorCount = 1;
		}
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_logicalDevice.getVulkanHandle(), m_vkDescriptorSetLayout, nullptr);
	}

	const std::vector<VulkanDescriptorSetLayout::Binding> & VulkanDescriptorSetLayout::getBindings() const
	{
		return m_bindings;
	}

	VkDescriptorSetLayout VulkanDescriptorSetLayout::getVulkanHandle() const
	{
		return m_vkDescriptorSetLayout;
	}

	VkWriteDescriptorSet VulkanDescriptorSetLayout::getVkWriteDescriptorSetForBinding(uint32_t binding) const
	{
		return m_vkWriteDescriptorSets[binding];
	}

	VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDescriptorSetLayout & layout)
		: m_layout(layout)
	{
		m_vkWriteImageInfo.resize(layout.getBindings().size(), VkDescriptorImageInfo{});
		m_vkWriteBufferInfo.resize(layout.getBindings().size(), VkDescriptorBufferInfo{});
	}

	VulkanDescriptorSet::VulkanDescriptorSet(VulkanDescriptorSet && other)
		: m_layout(other.m_layout)
	{ }

	const VulkanDescriptorSetLayout & VulkanDescriptorSet::getLayout()
	{
		return m_layout;
	}

	void VulkanDescriptorSet::setVulkanHandle(VkDescriptorSet vkDescriptorSet)
	{
		m_vkDescriptorSet = vkDescriptorSet;
	}

	VkDescriptorSet VulkanDescriptorSet::getVulkanHandle() const
	{
		return m_vkDescriptorSet;
	}

	VkDescriptorImageInfo * VulkanDescriptorSet::getVkWriteImageInfoForBinding(uint32_t binding)
	{
		return &m_vkWriteImageInfo[binding];
	}

	VkDescriptorBufferInfo * VulkanDescriptorSet::getVkWriteBufferInfoForBinding(uint32_t binding)
	{
		return &m_vkWriteBufferInfo[binding];
	}

	VulkanDescriptorSetManager::VulkanDescriptorSetManager(const VulkanLogicalDevice & logicalDevice)
		: m_logicalDevice(logicalDevice)
	{ }

	VulkanDescriptorSet * VulkanDescriptorSetManager::allocate(const VulkanDescriptorSetLayout & layout)
	{
		VulkanDescriptorSet * newSet = new VulkanDescriptorSet(layout);
		m_toAllocate.push_back(newSet);
		return newSet;
	}

	void VulkanDescriptorSetManager::update(VulkanDescriptorSet * set, uint32_t binding, VkDescriptorBufferInfo vkDescriptorBufferInfo)
	{
		m_toUpdate.push_back(set);
		m_pendingUpdateWrites.push_back(set->getLayout().getVkWriteDescriptorSetForBinding(binding));

		VkDescriptorBufferInfo * pBufferInfo = set->getVkWriteBufferInfoForBinding(binding);
		*pBufferInfo = vkDescriptorBufferInfo;
		m_pendingUpdateWrites.back().pBufferInfo = pBufferInfo;
	}

	void VulkanDescriptorSetManager::update(VulkanDescriptorSet * set, uint32_t binding, VkDescriptorImageInfo vkDescriptorImageInfo)
	{
		m_toUpdate.push_back(set);
		m_pendingUpdateWrites.push_back(set->getLayout().getVkWriteDescriptorSetForBinding(binding));

		VkDescriptorImageInfo * pImageInfo = set->getVkWriteImageInfoForBinding(binding);
		*pImageInfo = vkDescriptorImageInfo;
		m_pendingUpdateWrites.back().pImageInfo = pImageInfo;
	}

	void VulkanDescriptorSetManager::bind(VulkanDescriptorSet * set)
	{
		m_toBind.push_back(set);
	}

	void VulkanDescriptorSetManager::bind(VulkanDescriptorSet * set, uint32_t dynamicOffset)
	{
		bind(set);
		m_bindDynamicOffsets.push_back(dynamicOffset);
	}

	void VulkanDescriptorSetManager::free(VulkanDescriptorSet *& set)
	{
		m_toFree.push_back(&set);
	}

	void VulkanDescriptorSetManager::doAllocates()
	{
		if (m_toAllocate.size() == 0)
			return;

		std::vector<VkDescriptorSet> vkSets(m_toAllocate.size());
		std::vector<VkDescriptorSetLayout> vkLayouts(m_toAllocate.size());

		for (size_t idx = 0; idx < m_toAllocate.size(); ++idx)
			vkLayouts[idx] = m_toAllocate[idx]->getLayout().getVulkanHandle();

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_logicalDevice.getDescriptorPool();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(m_toAllocate.size());
		allocInfo.pSetLayouts = vkLayouts.data();

		if (MUD__checkVulkanCall(vkAllocateDescriptorSets(m_logicalDevice.getVulkanHandle(), &allocInfo, vkSets.data()), "Failed to allocate descriptor sets"))
		{
			for (size_t idx = 0; idx < m_toAllocate.size(); ++idx)
				m_toAllocate[idx]->setVulkanHandle(vkSets[idx]);
		}

		m_toAllocate.clear();
	}

	void VulkanDescriptorSetManager::doUpdates()
	{
		if (m_toUpdate.size() == 0)
			return;

		for (size_t idx = 0; idx < m_toUpdate.size(); ++idx)
			m_pendingUpdateWrites[idx].dstSet = m_toUpdate[idx]->getVulkanHandle();

		vkUpdateDescriptorSets(m_logicalDevice.getVulkanHandle(), static_cast<uint32_t>(m_pendingUpdateWrites.size()), m_pendingUpdateWrites.data(), 0, nullptr);

		m_toUpdate.clear();
		m_pendingUpdateWrites.clear();
	}

	void VulkanDescriptorSetManager::doBinds(VkCommandBuffer vkCommandBuffer, VkPipelineLayout vkPipelineLayout)
	{
		if (m_toBind.size() == 0)
			return;

		std::vector<VkDescriptorSet> vkDescriptorSets(m_toBind.size());

		for (size_t idx = 0; idx < m_toBind.size(); ++idx)
			vkDescriptorSets[idx] = m_toBind[idx]->getVulkanHandle();

		vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, 0, vkDescriptorSets.size(), vkDescriptorSets.data(), static_cast<uint32_t>(m_bindDynamicOffsets.size()), m_bindDynamicOffsets.data());

		m_toBind.clear();
		m_bindDynamicOffsets.clear();
	}

	void VulkanDescriptorSetManager::doFrees()
	{
		if (m_toFree.size() == 0)
			return;

		std::vector<VkDescriptorSet> vkDescriptorSets(m_toFree.size());

		for (size_t idx = 0; idx < m_toFree.size(); ++idx)
		{
			vkDescriptorSets[idx] = (*m_toFree[idx])->getVulkanHandle();
			delete (*m_toFree[idx]);
			*m_toFree[idx] = nullptr;
		}

		vkFreeDescriptorSets(m_logicalDevice.getVulkanHandle(), m_logicalDevice.getDescriptorPool(), vkDescriptorSets.size(), vkDescriptorSets.data());

		m_toFree.clear();
	}
}