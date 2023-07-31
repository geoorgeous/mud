#ifndef RENDER_PASS_HPP
#define RENDER_PASS_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_render_pass.hpp"
namespace mud
{
	using RenderPass = graphics_backend::vk::RenderPass;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/vulkan_render_pass.hpp"
namespace mud
{
	using RenderPass = graphics_backend::opengl::RenderPass;
}
#endif

#endif