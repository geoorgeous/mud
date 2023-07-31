#ifndef RENDER_SUB_PASS_HPP
#define RENDER_SUB_PASS_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_render_sub_pass.hpp"
namespace mud
{
	using RenderSubpass = graphics_backend::vk::RenderSubpass;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_render_sub_pass.hpp"
namespace mud
{
	using RenderSubpass = graphics_backend::opengl::RenderSubpass;
}
#endif

#endif