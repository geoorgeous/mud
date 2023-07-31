#include "vulkan_logical_device.hpp"

#include <array>
#include <set>
#define VMA_IMPLEMENTATION
#include <vulkan/vk_mem_alloc.h>

#include "utils/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_context.hpp"

namespace mud::graphics_backend::vk
{
	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		vkDestroyDescriptorPool(m_vkDevice, m_vkDescriptorPool, nullptr);

		vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, nullptr);

		vmaDestroyAllocator(m_vmaAllocator);

		vkDestroyDevice(m_vkDevice, nullptr);
	}

	void VulkanLogicalDevice::init(const VulkanContext & context, const VulkanPhysicalDevice & physicalDevice, const VulkanPhysicalDevice::QueueFamilyDetails & queueFamilyDetails, const std::vector<std::string> & enabledExtensions, const std::vector<std::string> & enabledLayers)
	{
		log(LogLevel::Trace, "Creating logical device...\n", "Vulkan");

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueQueueFamilies = {
			queueFamilyDetails.graphicsFamilyIndex.value(),
			queueFamilyDetails.presentFamilyIndex.value()
		};

		float queuePriority = 1.0f;

		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		std::vector<const char *> enabledExtensionsCStrs;
		for (const std::string & enabledExtension : enabledExtensions)
			enabledExtensionsCStrs.push_back(enabledExtension.data());

		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionsCStrs.size());
		deviceCreateInfo.ppEnabledExtensionNames = reinterpret_cast<const char *const *>(enabledExtensionsCStrs.data());

		std::vector<const char *> enabledLayersCStrs;
		for (const std::string & enabledLayer : enabledLayers)
			enabledLayersCStrs.push_back(enabledLayer.data());

		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayersCStrs.size());
		deviceCreateInfo.ppEnabledLayerNames = reinterpret_cast<const char *const *>(enabledLayersCStrs.data());

		if (!MUD__checkVulkanCall(vkCreateDevice(physicalDevice.getVulkanHandle(), &deviceCreateInfo, nullptr, &m_vkDevice), "Failed to create logical device"))
			return;

		vkGetDeviceQueue(m_vkDevice, queueFamilyDetails.graphicsFamilyIndex.value(), 0, &m_vkQueueGraphics);
		vkGetDeviceQueue(m_vkDevice, queueFamilyDetails.presentFamilyIndex.value(), 0, &m_vkQueuePresent);

		m_physicalDevice = &physicalDevice;
		
		log(LogLevel::Trace, "Creating allocator...\n", "Vulkan");

		VmaVulkanFunctions vmaVulkanFunctions = {
			vkGetInstanceProcAddr,
			vkGetDeviceProcAddr,
			vkGetPhysicalDeviceProperties,
			vkGetPhysicalDeviceMemoryProperties,
			vkAllocateMemory,
			vkFreeMemory,
			vkMapMemory,
			vkUnmapMemory,
			vkFlushMappedMemoryRanges,
			vkInvalidateMappedMemoryRanges,
			vkBindBufferMemory,
			vkBindImageMemory,
			vkGetBufferMemoryRequirements,
			vkGetImageMemoryRequirements,
			vkCreateBuffer,
			vkDestroyBuffer,
			vkCreateImage,
			vkDestroyImage,
			vkCmdCopyBuffer,
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
			vkGetBufferMemoryRequirements2,
			vkGetImageMemoryRequirements2,
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
			vkBindBufferMemory2,
			vkBindImageMemory2,
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
			vkGetPhysicalDeviceMemoryProperties2,
#endif
#if VMA_VULKAN_VERSION >= 1003000
			vkGetDeviceBufferMemoryRequirements,
			vkGetDeviceImageMemoryRequirements
#endif
		};

		VmaAllocatorCreateInfo vmaAllocatorCreateInfo;
		vmaAllocatorCreateInfo.flags = 0;
		vmaAllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
		vmaAllocatorCreateInfo.instance = context.getVulkanHandle();
		vmaAllocatorCreateInfo.physicalDevice = m_physicalDevice->getVulkanHandle();
		vmaAllocatorCreateInfo.device = m_vkDevice;
		vmaAllocatorCreateInfo.pVulkanFunctions = VMA_NULL;
		vmaAllocatorCreateInfo.pAllocationCallbacks = VMA_NULL;
		vmaAllocatorCreateInfo.pDeviceMemoryCallbacks = VMA_NULL;
		vmaAllocatorCreateInfo.pHeapSizeLimit = VMA_NULL;

		if (!MUD__checkVulkanCall(vmaCreateAllocator(&vmaAllocatorCreateInfo, &m_vmaAllocator), "Failed to create allocator for logical device"))
			return;

		// create pools

		log(LogLevel::Trace, "Creating device pools...\n", "Vulkan");

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice->getVulkanHandle(), &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice->getVulkanHandle(), &queueFamilyCount, queueFamilies.data());

		uint32_t graphicsQueueFamilyIndex = 0;
		for (size_t idx = 0; idx < queueFamilies.size(); ++idx)
			if (queueFamilies[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueFamilyIndex = idx;
				break;
			}

		VkCommandPoolCreateInfo vkCommanPoolCreateInfo{};
		vkCommanPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vkCommanPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		vkCommanPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

		MUD__checkVulkanCall(vkCreateCommandPool(m_vkDevice, &vkCommanPoolCreateInfo, nullptr, &m_vkCommandPool), "Failed to create command pool");

		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(256);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(256);

		VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo{};
		vkDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		vkDescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		vkDescriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
		vkDescriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(256);
		vkDescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		MUD__checkVulkanCall(vkCreateDescriptorPool(m_vkDevice, &vkDescriptorPoolCreateInfo, nullptr, &m_vkDescriptorPool), "Failed to create descriptor pool");
	}

	const VulkanPhysicalDevice * VulkanLogicalDevice::getPhysicalDevice() const
	{
		return m_physicalDevice;
	}

	VkDevice VulkanLogicalDevice::getVulkanHandle() const
	{
		return m_vkDevice;
	}

	VkQueue VulkanLogicalDevice::getGraphicsQueue() const
	{
		return m_vkQueueGraphics;
	}

	VkQueue VulkanLogicalDevice::getPresentQueue() const
	{
		return m_vkQueuePresent;
	}

	const VmaAllocator & VulkanLogicalDevice::getAllocator() const
	{
		return m_vmaAllocator;
	}

	const VkDescriptorPool & VulkanLogicalDevice::getDescriptorPool() const
	{
		return m_vkDescriptorPool;
	}

	const VkCommandPool & VulkanLogicalDevice::getCommandPool() const
	{
		return m_vkCommandPool;
	}

	VkCommandBuffer VulkanLogicalDevice::beginSingleTimeCommands() const
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_vkCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_vkDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanLogicalDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer) const
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_vkQueueGraphics, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_vkQueueGraphics);

		vkFreeCommandBuffers(m_vkDevice, m_vkCommandPool, 1, &commandBuffer);
	}

	bool VulkanLogicalDevice::createDescriptorSets(std::vector<VkDescriptorSet> & descriptorSets, const VkDescriptorSetLayout & descriptorSetLayout)
	{
		std::vector<VkDescriptorSetLayout> layouts(2, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_vkDescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(2);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(allocInfo.descriptorSetCount);

		return MUD__checkVulkanCall(vkAllocateDescriptorSets(m_vkDevice, &allocInfo, descriptorSets.data()), "Failed to allocate descriptor sets");
	}
}