#ifndef MESH_HPP
#define MESH_HPP

#if defined(MUD_USE_VULKAN)
#include "backend/vulkan/vulkan_mesh.hpp"
namespace mud
{
	using Mesh = graphics_backend::vk::Mesh;
}
#elif defined(MUD_USE_OPENGL)
#include "backend/opengl/opengl_mesh.hpp"
namespace mud
{
	using Mesh = graphics_backend::opengl::Mesh;
}
#endif

#endif