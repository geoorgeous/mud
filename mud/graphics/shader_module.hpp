#ifndef SHADER_MODULE_HPP
#define SHADER_MODULE_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_shader_module.hpp"
namespace mud
{
	using ShaderModule = graphics_backend::vk::VulkanShaderModule;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_shader_module.hpp"
namespace mud
{
	using ShaderModule = graphics_backend::opengl::OpenGLShaderModule;
}
#endif

#endif