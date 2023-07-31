#ifndef VULKAN_WINDOW_GRAPHICS_CONTEXT_HPP
#define VULKAN_WINDOW_GRAPHICS_CONTEXT_HPP

#include "graphics/interface/window_graphics_context_base.hpp"
#include "internal/vulkan_surface.hpp"
#include "internal/vulkan_swapchain.hpp"

namespace mud::graphics_backend::vk
{
	class VulkanWindowGraphicsContext : public mud::WindowGraphicsContextBase
	{
	public:

		~VulkanWindowGraphicsContext();

		VulkanSurface & getSurface();

		VulkanSwapchain & getSwapchain();

		virtual void makeDirty() override;

		virtual bool beginDraw() override;

		virtual void endDraw() override;

	private:

		VulkanSurface m_surface;

		VulkanSwapchain m_swapchain;
	};
}

#endif