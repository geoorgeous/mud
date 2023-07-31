#ifndef DEFERRED_RENDERER_HPP
#define DEFERRED_RENDERER_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_deferred_renderer.hpp"
namespace mud
{
	using DeferredRenderer = graphics_backend::vk::VulkanDeferredRenderer;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_deferred_renderer.hpp"
namespace mud
{
	using DeferredRenderer = graphics_backend::opengl::OpenGLDeferredRenderer;
}
#endif

#endif