#include "vulkan_swapchain.hpp"

#include <algorithm>
#include <array>

#include "utils/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_image.hpp"
#include "vulkan_logical_device.hpp"
#include "vulkan_physical_device.hpp"
#include "vulkan_surface.hpp"

namespace mud::graphics_backend::vk
{
	VulkanSwapchain::~VulkanSwapchain()
	{
		destroy();
	}

	bool VulkanSwapchain::init(const VulkanLogicalDevice & logicalDevice, const VulkanSurface &surface)
	{
		VkExtent2D surfaceExtent = surface.getExtent2D();
		if (surfaceExtent.width == 0 || surfaceExtent.height == 0)
			return false;

		// Check the physical device supports swapchain features

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(logicalDevice.getPhysicalDevice()->getVulkanHandle(), surface.getVulkanHandle(), &formatCount, nullptr);

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(logicalDevice.getPhysicalDevice()->getVulkanHandle(), surface.getVulkanHandle(), &presentModeCount, nullptr);

		if (formatCount == 0 || presentModeCount == 0)
		{
			log(LogLevel::Info, "Required SwapChain capabilities not supported\n", "Vulkan");
			return false;
		}

		VulkanPhysicalDevice::SurfaceDetails physicalDeviceSurfaceDetails = logicalDevice.getPhysicalDevice()->getAvailableSurfaceDetails(surface);

		uint32_t imageCount = physicalDeviceSurfaceDetails.vkSurfaceCapabilities.minImageCount + 1;
		if (physicalDeviceSurfaceDetails.vkSurfaceCapabilities.maxImageCount > 0 && imageCount > physicalDeviceSurfaceDetails.vkSurfaceCapabilities.maxImageCount)
		{
			imageCount = physicalDeviceSurfaceDetails.vkSurfaceCapabilities.maxImageCount;
		}

		VkFormat swapchainImageFormat;
		VkColorSpaceKHR swapchainImageColorSpace;
		for (const auto & availableFormat : physicalDeviceSurfaceDetails.vkSurfaceFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB)
				swapchainImageFormat = availableFormat.format;
			if (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				swapchainImageColorSpace = availableFormat.colorSpace;
		}

		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto & availablePresentMode : physicalDeviceSurfaceDetails.vkPresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				swapchainPresentMode = availablePresentMode;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface.getVulkanHandle();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_vkFormat = swapchainImageFormat;
		createInfo.imageColorSpace = swapchainImageColorSpace;
		createInfo.imageExtent = m_vkExtent2D = surface.getExtent2D();
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // It is also possible that you'll render images to a separate image first to perform operations like post-processing. In that case you may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image.
		createInfo.preTransform = physicalDeviceSurfaceDetails.vkSurfaceCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = swapchainPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VulkanPhysicalDevice::QueueFamilyDetails physicalDeviceQueueFamilyDetails = logicalDevice.getPhysicalDevice()->getQueueFamilyDetails(surface);
		uint32_t queueFamilyIndices[] = {
			physicalDeviceQueueFamilyDetails.graphicsFamilyIndex.value(),
			physicalDeviceQueueFamilyDetails.presentFamilyIndex.value()
		};

		if (physicalDeviceQueueFamilyDetails.graphicsFamilyIndex == physicalDeviceQueueFamilyDetails.presentFamilyIndex)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		
		log(LogLevel::Trace, fmt::format("Creating swap chain with {0} image(s) at {1} x {2}...\n", imageCount, m_vkExtent2D.width, m_vkExtent2D.height), "Vulkan");

		if (!MUD__checkVulkanCall(vkCreateSwapchainKHR(logicalDevice.getVulkanHandle(), &createInfo, nullptr, &m_vkSwapchain), "Failed to create swap chain"))
		{
			return false;
		}

		vkGetSwapchainImagesKHR(logicalDevice.getVulkanHandle(), m_vkSwapchain, &imageCount, nullptr);
		m_images.resize(imageCount);
		std::vector<VkImage> vkImages(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice.getVulkanHandle(), m_vkSwapchain, &imageCount, vkImages.data());

		for (size_t idx = 0; idx < vkImages.size(); ++idx)
		{
			m_images[idx].vkImage = vkImages[idx];

			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = vkImages[idx];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_vkFormat;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			if (!MUD__checkVulkanCall(vkCreateImageView(logicalDevice.getVulkanHandle(), &imageViewCreateInfo, nullptr, &m_images[idx].vkImageView), "Failed to create image view"))
			{
				return false;
			}
		}

		VkFormat depthFormat = logicalDevice.getPhysicalDevice()->findSupportedDepthFormat();
		m_depthImage = new VulkanImage(logicalDevice, m_vkExtent2D.width, m_vkExtent2D.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_vkDepthImageView = m_depthImage->createView(VK_IMAGE_ASPECT_DEPTH_BIT);

		// setup frame info

		m_frames.resize(imageCount);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = logicalDevice.getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = imageCount;

		for (size_t idx = 0; idx < imageCount; ++idx)
		{
			if (!MUD__checkVulkanCall(vkAllocateCommandBuffers(logicalDevice.getVulkanHandle(), &allocInfo, &m_frames[idx].commandBuffer), "Failed to allocate command buffer(s)"))
			{
				return false;
			}
		}

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t idx = 0; idx < imageCount; ++idx)
		{
			if (!MUD__checkVulkanCall(vkCreateSemaphore(logicalDevice.getVulkanHandle(), &semaphoreInfo, nullptr, &m_frames[idx].semaphoreImageAvailable), "Failed to create ImageAvailable semaphore for swapchain frame") ||
				!MUD__checkVulkanCall(vkCreateSemaphore(logicalDevice.getVulkanHandle(), &semaphoreInfo, nullptr, &m_frames[idx].semaphoreRenderFinished), "Failed to create RenderFinished semaphore for swapchain frame") ||
				!MUD__checkVulkanCall(vkCreateFence(logicalDevice.getVulkanHandle(), &fenceInfo, nullptr, &m_frames[idx].fenceInFlight), "Failed to create fence for swapchain frame"))
			{
				return false;
			}

			m_frames[idx].index = idx;
		}
		
		m_logicalDevice = &logicalDevice;
		m_surface = &surface;

		m_currentFrameInfo = m_frames.begin();

		m_isDirty = false;

		m_eventOnReconstructed.broadcast();

		return true;
	}

	VkSwapchainKHR VulkanSwapchain::getVulkanHandle() const
	{
		return m_vkSwapchain;
	}

	const VulkanLogicalDevice * VulkanSwapchain::getLogicalDevice() const
	{
		return m_logicalDevice;
	}

	void VulkanSwapchain::createRenderPassFramebuffers(std::vector<VkFramebuffer> & vkFramebuffers, VkRenderPass vkRenderPass) const
	{
		vkFramebuffers.resize(m_images.size());
		for (size_t idx = 0; idx < m_images.size(); ++idx)
		{
			std::array<VkImageView, 2> attachments = {
				m_images[idx].vkImageView,
				m_vkDepthImageView
			};

			VkFramebufferCreateInfo vkFramebufferCreateInfo = {};
			vkFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			vkFramebufferCreateInfo.renderPass = vkRenderPass;
			vkFramebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
			vkFramebufferCreateInfo.pAttachments = attachments.data();
			vkFramebufferCreateInfo.width = m_vkExtent2D.width;
			vkFramebufferCreateInfo.height = m_vkExtent2D.height;
			vkFramebufferCreateInfo.layers = 1;

			MUD__checkVulkanCall(vkCreateFramebuffer(m_logicalDevice->getVulkanHandle(), &vkFramebufferCreateInfo, nullptr, &vkFramebuffers[idx]), "Failed to create framebuffer");
		}
	}

	const std::vector<VulkanSwapchain::Image> & VulkanSwapchain::getImages() const
	{
		return m_images;
	}

	VkImageView VulkanSwapchain::getDepthImageView() const
	{
		return m_vkDepthImageView;
	}

	const VulkanSwapchain::FrameInfo & VulkanSwapchain::getCurrentFrameInfo() const
	{
		return *m_currentFrameInfo;
	}

	VkFormat VulkanSwapchain::getImageFormat() const
	{
		return m_vkFormat;
	}

	VkExtent2D VulkanSwapchain::getImageExtent2D() const
	{
		return m_vkExtent2D;
	}

	bool VulkanSwapchain::getIsDirty() const
	{
		return m_isDirty;
	}

	void VulkanSwapchain::makeDirty()
	{
		m_isDirty = true;
	}

	const VulkanSwapchain::FrameInfo * VulkanSwapchain::beginFrame()
	{
		if (m_vkSwapchain == VK_NULL_HANDLE && !init(*m_logicalDevice, *m_surface))
			return nullptr;

		vkWaitForFences(m_logicalDevice->getVulkanHandle(), 1, &m_currentFrameInfo->fenceInFlight, VK_TRUE, UINT64_MAX);

		if (m_isDirty && !renew())
			return nullptr;

		VkResult result = vkAcquireNextImageKHR(m_logicalDevice->getVulkanHandle(), m_vkSwapchain, UINT64_MAX, m_currentFrameInfo->semaphoreImageAvailable, VK_NULL_HANDLE, &m_currentFrameInfo->imageIndex);

		if (result != VK_SUCCESS)
		{
			if (result == VK_SUBOPTIMAL_KHR)
				makeDirty();
			else
			{
				log(LogLevel::Error, fmt::format("Failed to acquire swap chain image for presentation\n"), "Vulkan");
				return nullptr;
			}
		}

		vkResetFences(m_logicalDevice->getVulkanHandle(), 1, &m_currentFrameInfo->fenceInFlight);

		vkResetCommandBuffer(m_currentFrameInfo->commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;				  // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		MUD__checkVulkanCall(vkBeginCommandBuffer(m_currentFrameInfo->commandBuffer, &beginInfo), "Failed to begin recording command buffer");

		return &*m_currentFrameInfo;
	}

	void VulkanSwapchain::endFrame()
	{
		MUD__checkVulkanCall(vkEndCommandBuffer(m_currentFrameInfo->commandBuffer), "Failed to end command buffer recording");

		presentFrame(*m_currentFrameInfo);

		if (++m_currentFrameInfo == m_frames.end())
			m_currentFrameInfo = m_frames.begin();
	}

	void VulkanSwapchain::waitUntilIdle() const
	{
		std::vector<VkFence> fences;
		for (const FrameInfo & frameInfo : m_frames)
		{
			fences.push_back(frameInfo.fenceInFlight);
		}
		vkWaitForFences(m_logicalDevice->getVulkanHandle(), fences.size(), fences.data(), VK_TRUE, 1000000000ull * 3);
	}
	
	VulkanSwapchain::EventOnReconstructed & VulkanSwapchain::getEventOnReconstructed()
	{
		return m_eventOnReconstructed;
	}
	
	void VulkanSwapchain::destroy()
	{
		waitUntilIdle();

		for (FrameInfo & frameInfo : m_frames)
		{
			vkDestroySemaphore(m_logicalDevice->getVulkanHandle(), frameInfo.semaphoreImageAvailable, nullptr);
			vkDestroySemaphore(m_logicalDevice->getVulkanHandle(), frameInfo.semaphoreRenderFinished, nullptr);
			vkDestroyFence(m_logicalDevice->getVulkanHandle(), frameInfo.fenceInFlight, nullptr);
		}

		for (Image & image : m_images)
			vkDestroyImageView(m_logicalDevice->getVulkanHandle(), image.vkImageView, nullptr);

		vkDestroyImageView(m_logicalDevice->getVulkanHandle(), m_vkDepthImageView, nullptr);
		delete m_depthImage;

		vkDestroySwapchainKHR(m_logicalDevice->getVulkanHandle(), m_vkSwapchain, nullptr);
		m_vkSwapchain = VK_NULL_HANDLE;
	}

	bool VulkanSwapchain::renew()
	{
		destroy();
		return init(*m_logicalDevice, *m_surface);
	}

	void VulkanSwapchain::presentFrame(FrameInfo & frameInfo)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {frameInfo.semaphoreImageAvailable};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frameInfo.commandBuffer;
		VkSemaphore signalSemaphores[] = {frameInfo.semaphoreRenderFinished};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		MUD__checkVulkanCall(vkQueueSubmit(m_logicalDevice->getGraphicsQueue(), 1, &submitInfo, frameInfo.fenceInFlight), "Failed to submit draw command buffer");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_vkSwapchain;
		presentInfo.pImageIndices = &frameInfo.imageIndex;

		VkResult result = vkQueuePresentKHR(m_logicalDevice->getPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			log(LogLevel::Trace, "Swapchain out of date or suboptimal. Making swapchain dirty\n", "VulkanSwapchain");
			m_isDirty = true;
			return;
		}
		else if (result != VK_SUCCESS)
		{
			log(LogLevel::Error, "Failed to present swap chain\n", "VulkanSwapchain");
			return;
		}
	}
}