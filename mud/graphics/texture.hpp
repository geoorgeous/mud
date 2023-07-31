#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_texture.hpp"
namespace mud
{
	using Texture = graphics_backend::vk::VulkanTexture;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_texture.hpp"
namespace mud
{
	using Texture = graphics_backend::opengl::OpenGLTexture;
}
#endif

#endif