#include "mesh_base.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fmt/format.h>
#include <iterator>

#include "utils/logger.hpp"

namespace mud
{
	MeshBase::MeshBase()
	{ }

	MeshBase::MeshBase(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> & indices)
		: MeshBase()
	{
		setData(vertices, indices);
	}

	const std::vector<MeshVertex> & MeshBase::getVertices() const
	{
		return m_vertices;
	}

	const std::vector<uint32_t> & MeshBase::getIndices() const
	{
		return m_indices;
	}

	const AABB & MeshBase::getBoundingBox() const
	{
		return m_boundingBox;
	}

	void MeshBase::setData(const std::vector<MeshVertex> & vertices, const std::vector<uint32_t> & indices)
	{
		m_vertices = vertices;
		m_indices = indices;
		recalculateBoundingBox();
		onSetData();
	}

	bool MeshBase::deserialize(std::ifstream & file)
	{
		if (!serialization_helpers::deserializeVector(file, m_vertices) || !serialization_helpers::deserializeVector(file, m_indices))
			return false;

		recalculateBoundingBox();
		onSetData();

		return true;
	}

	bool MeshBase::serialize(std::ofstream & file) const
	{
		return serialization_helpers::serializeVector(file, m_vertices) &&
			serialization_helpers::serializeVector(file, m_indices);
	}

	void MeshBase::recalculateBoundingBox()
	{
		if (m_vertices.size() == 0)
		{
			m_boundingBox.min = m_boundingBox.max = Vector3::zero;
			return;
		}

		m_boundingBox.min = m_boundingBox.max = m_vertices[0].position;

		for (const MeshVertex & vertex : m_vertices)
		{
			if (vertex.position.x < m_boundingBox.min.x)
				m_boundingBox.min.x = vertex.position.x;
			else if (vertex.position.x > m_boundingBox.max.x)
				m_boundingBox.max.x = vertex.position.x;

			if (vertex.position.y < m_boundingBox.min.y)
				m_boundingBox.min.y = vertex.position.y;
			else if (vertex.position.y > m_boundingBox.max.y)
				m_boundingBox.max.y = vertex.position.y;

			if (vertex.position.z < m_boundingBox.min.z)
				m_boundingBox.min.z = vertex.position.z;
			else if (vertex.position.z > m_boundingBox.max.z)
				m_boundingBox.max.z = vertex.position.z;
		}
	}
}