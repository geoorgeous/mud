#ifndef APPLICATION_GRAPHICS_CONTEXT_HPP
#define APPLICATION_GRAPHICS_CONTEXT_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_application_graphics_context.hpp"
namespace mud
{
	using ApplicationGraphicsContext = graphics_backend::vk::VulkanApplicationGraphicsContext;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_application_graphics_context.hpp"
namespace mud
{
	using ApplicationGraphicsContext = graphics_backend::opengl::OpenGLApplicationGraphicsContext;
}
#endif

#endif