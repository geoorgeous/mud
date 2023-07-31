#include "vulkan_mesh.hpp"

#include "utils/logger.hpp"
#include "vulkan_application_graphics_context.hpp"

namespace mud::graphics_backend::vk
{
	VulkanVertexInputInfo Mesh::getVertexInputInfo()
	{
		static VulkanVertexInputInfo vertexInputInfo;

		vertexInputInfo.vkBindingDescription.binding = 0;
		vertexInputInfo.vkBindingDescription.stride = sizeof(MeshVertex);
		vertexInputInfo.vkBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		vertexInputInfo.vkAttributeDescriptions.resize(4);

		vertexInputInfo.vkAttributeDescriptions[0].binding = 0;
		vertexInputInfo.vkAttributeDescriptions[0].location = 0;
		vertexInputInfo.vkAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputInfo.vkAttributeDescriptions[0].offset = offsetof(MeshVertex, position);

		vertexInputInfo.vkAttributeDescriptions[1].binding = 0;
		vertexInputInfo.vkAttributeDescriptions[1].location = 1;
		vertexInputInfo.vkAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputInfo.vkAttributeDescriptions[1].offset = offsetof(MeshVertex, normal);

		vertexInputInfo.vkAttributeDescriptions[2].binding = 0;
		vertexInputInfo.vkAttributeDescriptions[2].location = 2;
		vertexInputInfo.vkAttributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexInputInfo.vkAttributeDescriptions[2].offset = offsetof(MeshVertex, colour);

		vertexInputInfo.vkAttributeDescriptions[3].binding = 0;
		vertexInputInfo.vkAttributeDescriptions[3].location = 3;
		vertexInputInfo.vkAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputInfo.vkAttributeDescriptions[3].offset = offsetof(MeshVertex, textureCoordinates);

		return vertexInputInfo;
	}

	Mesh::Mesh()
		: MeshBase(), m_vertexBuffer(nullptr), m_indexBuffer(nullptr)
	{ }

	Mesh::Mesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> & indices)
	{
		setData(vertices, indices);
	}

	Mesh::~Mesh()
	{
		delete m_vertexBuffer;
		delete m_indexBuffer;
	}

	const VulkanBuffer * Mesh::getVertexBuffer() const
	{
		return m_vertexBuffer;
	}

	const VulkanBuffer * Mesh::getIndexBuffer() const
	{
		return m_indexBuffer;
	}

	void Mesh::onSetData()
	{
		VulkanApplicationGraphicsContext * vulkan = VulkanApplicationGraphicsContext::getInstance();

		size_t bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

		if (m_vertexBuffer == nullptr || m_vertexBuffer->getSize() != bufferSize)
		{
			delete m_vertexBuffer;
			m_vertexBuffer = new VulkanBuffer(vulkan->getLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		}
		if (m_vertices.size() > 0)
		{
			VulkanBuffer vertexStagingBuffer(vulkan->getLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			vertexStagingBuffer.set(0, bufferSize, m_vertices.data());
			vertexStagingBuffer.copy(*m_vertexBuffer);
		}

		bufferSize = sizeof(m_indices[0]) * m_indices.size();

		if (m_indexBuffer == nullptr || m_indexBuffer->getSize() != bufferSize)
		{
			delete m_indexBuffer;
			m_indexBuffer = new VulkanBuffer(vulkan->getLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		}
		if (m_indices.size() > 0)
		{
			VulkanBuffer indexStagingBuffer(vulkan->getLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			indexStagingBuffer.set(0, bufferSize, m_indices.data());
			indexStagingBuffer.copy(*m_indexBuffer);
		}

	}
}