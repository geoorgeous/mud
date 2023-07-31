#ifndef VULKAN_MESH_HPP
#define VULKAN_MESH_HPP

#include <vulkan/vulkan.h>

#include "graphics/interface/mesh_base.hpp"
#include "internal/vulkan_buffer.hpp"
#include "vulkan_vertex_input_info.hpp"

namespace mud::graphics_backend::vk
{
	class Mesh : public mud::MeshBase
	{
	public:

		static VulkanVertexInputInfo getVertexInputInfo();

		Mesh();

		Mesh(const std::vector<MeshVertex> & vertices, const std::vector<uint32_t> & indices = {});

		~Mesh();

		Mesh(const Mesh &) = delete;
		Mesh &operator=(const Mesh &) = delete;

		const VulkanBuffer * getVertexBuffer() const;

		const VulkanBuffer * getIndexBuffer() const;

		virtual void onSetData() override;

	private:

		VulkanBuffer * m_vertexBuffer;
		VulkanBuffer * m_indexBuffer;
	};
}

#endif