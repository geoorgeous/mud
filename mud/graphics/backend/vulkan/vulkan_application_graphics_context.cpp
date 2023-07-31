#include "vulkan_application_graphics_context.hpp"

#include <cstring>
#include <fmt/format.h>
#include <glfw/glfw3.h>
#include <set>

#include "graphics/application_graphics_context.hpp"
#include "graphics/backend/spirv/spirv.hpp"
#include "graphics/window_graphics_context.hpp"
#include "internal/vulkan_debug.hpp"
#include "platform/window.hpp"
#include "utils/file_io.hpp"
#include "utils/logger.hpp"

namespace mud::graphics_backend::vk
{
	VulkanApplicationGraphicsContext * VulkanApplicationGraphicsContext::s_instance = nullptr;

	VulkanApplicationGraphicsContext * VulkanApplicationGraphicsContext::getInstance()
	{
		return s_instance;
	}

	void VulkanApplicationGraphicsContext::init(const WindowProperties & windowProperties)
	{
		m_mainWindow = new Window(windowProperties);
		WindowGraphicsContext & mainWindowGraphicsContext = m_mainWindow->getGraphicsContext();

		uint32_t apiVersion;
		vkEnumerateInstanceVersion(&apiVersion);
		log(LogLevel::Info, fmt::format("Vulkan API: {0}.{1}.{2}\n", VK_API_VERSION_MAJOR(apiVersion), VK_API_VERSION_MINOR(apiVersion), VK_API_VERSION_PATCH(apiVersion)), "Vulkan");

		m_context.init(k_layers);
		m_physicalDevice.init(m_context, VulkanPhysicalDevice::Requirements{ k_deviceExtensions });
		mainWindowGraphicsContext.getSurface().init(m_context, *m_mainWindow);
		m_logicalDevice.init(m_context, m_physicalDevice, m_physicalDevice.getQueueFamilyDetails(mainWindowGraphicsContext.getSurface()), k_deviceExtensions, k_layers);
		mainWindowGraphicsContext.getSwapchain().init(m_logicalDevice, mainWindowGraphicsContext.getSurface());

		s_instance = this;
	}

	VulkanApplicationGraphicsContext::~VulkanApplicationGraphicsContext()
	{
		delete m_mainWindow;
		vkDeviceWaitIdle(m_logicalDevice.getVulkanHandle());
	}

	Window * VulkanApplicationGraphicsContext::getMainWindow()
	{
		return m_mainWindow;
	}

	const VulkanLogicalDevice & VulkanApplicationGraphicsContext::getLogicalDevice() const
	{
		return m_logicalDevice;
	}
}