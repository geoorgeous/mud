#include "vulkan_window_graphics_context.hpp"

#include "graphics/application_graphics_context.hpp"

namespace mud::graphics_backend::vk
{
	VulkanWindowGraphicsContext::~VulkanWindowGraphicsContext()
	{
		m_swapchain.waitUntilIdle();
	}

	VulkanSurface & VulkanWindowGraphicsContext::getSurface()
	{
		return m_surface;
	}

	VulkanSwapchain & VulkanWindowGraphicsContext::getSwapchain()
	{
		return m_swapchain;
	}

	void VulkanWindowGraphicsContext::makeDirty()
	{
		m_swapchain.makeDirty();
	}

	bool VulkanWindowGraphicsContext::beginDraw()
	{
		return m_swapchain.beginFrame() != nullptr;
	}

	void VulkanWindowGraphicsContext::endDraw()
	{
		m_swapchain.endFrame();
	}
}