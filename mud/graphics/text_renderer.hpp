#ifndef TEXT_RENDERER_HPP
#define TEXT_RENDERER_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_text_renderer.hpp"
namespace mud
{
	using TextRenderer = graphics_backend::vk::VulkanTextRenderer;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_text_renderer.hpp"
namespace mud
{
	using TextRenderer = graphics_backend::opengl::OpenGLTextRenderer;
}
#endif

#endif