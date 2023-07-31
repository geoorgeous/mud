#ifndef VULKAN_SURFACE_HPP
#define VULKAN_SURFACE_HPP

#include <vector>
#include <vulkan/vulkan.h>

namespace mud
{
	class Window;

	namespace graphics_backend::vk
	{
		class VulkanContext;

		class VulkanSurface
		{
		public:

			~VulkanSurface();

			void init(const VulkanContext & context, const Window & window);

			VkSurfaceKHR getVulkanHandle() const;

			VkExtent2D getExtent2D() const;

		private:

			VkSurfaceKHR m_vkSurface;

			const VulkanContext * m_context;

			const Window * m_window;
		};
	}
}

#endif