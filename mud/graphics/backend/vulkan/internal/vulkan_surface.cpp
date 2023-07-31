#include "vulkan_surface.hpp"

#include <glfw/glfw3.h>

#include "platform/window.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_context.hpp"

namespace mud::graphics_backend::vk
{
	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(m_context->getVulkanHandle(), m_vkSurface, nullptr);
	}

	void VulkanSurface::init(const VulkanContext & context, const Window & window)
	{
		if (!MUD__checkVulkanCall(glfwCreateWindowSurface(context.getVulkanHandle(), window.getGLFWHandle(), nullptr, &m_vkSurface), "Failed to create window surface"))
			return;

		m_context = &context;
		m_window = &window;
	}

	VkSurfaceKHR VulkanSurface::getVulkanHandle() const
	{
		return m_vkSurface;
	}

	VkExtent2D VulkanSurface::getExtent2D() const
	{
		int width, height;
		glfwGetFramebufferSize(m_window->getGLFWHandle(), &width, &height);

		return {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
	}
}