#ifndef SPRITE_BATCH_RENDERER_HPP
#define SPRITE_BATCH_RENDERER_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_sprite_batch_renderer.hpp"
namespace mud
{
	using SpriteBatchRenderer = graphics_backend::vk::VulkanSpriteBatchRenderer;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_sprite_batch_renderer.hpp"
namespace mud
{
	using SpriteBatchRenderer = graphics_backend::opengl::OpenGLSpriteBatchRenderer;
}
#endif

#endif