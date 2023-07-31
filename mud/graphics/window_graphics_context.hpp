#ifndef WINDOW_GRAPHICS_CONTEXT_HPP
#define WINDOW_GRAPHICS_CONTEXT_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_window_graphics_context.hpp"
namespace mud
{
	using WindowGraphicsContext = graphics_backend::vk::VulkanWindowGraphicsContext;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_window_graphics_context.hpp"
namespace mud
{
	using WindowGraphicsContext = graphics_backend::opengl::OpenGLWindowGraphicsContext;
}
#endif

#endif