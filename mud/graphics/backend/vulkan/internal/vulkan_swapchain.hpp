#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include <vector>
#include <vulkan/vulkan.h>

#include "platform/event.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanImage;
	class VulkanLogicalDevice;
	class VulkanPhysicalDevice;
	class VulkanRenderPass;
	class VulkanSurface;

	class VulkanSwapchain
	{
	public:

		struct Image
		{
			VkImage vkImage;
			VkImageView vkImageView;
		};

		struct FrameInfo
		{
			size_t index;
			VkCommandBuffer commandBuffer;
			VkSemaphore semaphoreImageAvailable;
			VkSemaphore semaphoreRenderFinished;
			VkFence fenceInFlight;
			uint32_t imageIndex;
		};

		~VulkanSwapchain();

		bool init(const VulkanLogicalDevice & logicalDevice, const VulkanSurface & surface);

		VkSwapchainKHR getVulkanHandle() const;

		const VulkanLogicalDevice * getLogicalDevice() const;

		void createRenderPassFramebuffers(std::vector<VkFramebuffer> & vkFramebuffers, VkRenderPass vkRenderPass) const;

		const std::vector<Image> & getImages() const;

		VkImageView getDepthImageView() const;

		const FrameInfo & getCurrentFrameInfo() const;

		VkFormat getImageFormat() const;

		VkExtent2D getImageExtent2D() const;

		bool getIsDirty() const;

		void makeDirty();

		const VulkanSwapchain::FrameInfo * beginFrame();

		void endFrame();

		void waitUntilIdle() const;

		typedef Event<> EventOnReconstructed;
		EventOnReconstructed & getEventOnReconstructed();

	private:

		VkSwapchainKHR m_vkSwapchain;

		const VulkanLogicalDevice * m_logicalDevice;
		const VulkanSurface * m_surface;

		VkPresentModeKHR presentMode;
		VkFramebufferCreateInfo m_vkFramebufferCreateInfo;
		std::vector<Image> m_images;
		std::vector<FrameInfo> m_frames;
		std::vector<FrameInfo>::iterator m_currentFrameInfo;
		VkFormat m_vkFormat;
		VkExtent2D m_vkExtent2D;

		VulkanImage * m_depthImage;
		VkImageView m_vkDepthImageView;

		bool m_isDirty;

		EventOnReconstructed m_eventOnReconstructed;

		void destroy();

		bool renew();

		void presentFrame(FrameInfo & frameInfo);
	};
}

#endif