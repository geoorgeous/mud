#ifndef VULKAN_APPLICATION_GRAPHICS_CONTEXT_HPP
#define VULKAN_APPLICATION_GRAPHICS_CONTEXT_HPP

#include <array>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

#include "graphics/interface/application_graphics_context_base.hpp"
#include "internal/vulkan_context.hpp"
#include "internal/vulkan_logical_device.hpp"
#include "internal/vulkan_physical_device.hpp"
#include "math/vector.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanApplicationGraphicsContext : public ApplicationGraphicsContextBase
	{
	public:

		static VulkanApplicationGraphicsContext * getInstance();

		~VulkanApplicationGraphicsContext();

		virtual void init(const WindowProperties & windowProperties = WindowProperties()) override;

		Window * getMainWindow();

		const VulkanLogicalDevice & getLogicalDevice() const;

	private:

		static VulkanApplicationGraphicsContext * s_instance;

		const std::vector<std::string> k_layers {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<std::string> k_deviceExtensions {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		Window * m_mainWindow;
		VulkanContext m_context;
		VulkanPhysicalDevice m_physicalDevice;
		VulkanLogicalDevice m_logicalDevice;
	};
}

#endif