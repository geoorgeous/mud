#ifndef FORWARD_RENDERER_HPP
#define FORWARD_RENDERER_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_forward_renderer.hpp"
namespace mud
{
	using ForwardRenderer = graphics_backend::vk::VulkanForwardRenderer;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_forward_renderer.hpp"
namespace mud
{
	using ForwardRenderer = graphics_backend::opengl::OpenGLForwardRenderer;
}
#endif

#endif