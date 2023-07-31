#include "mesh_factory.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "math/matrix.hpp"
#include "texture.hpp"
#include "utils/logger.hpp"

namespace mud::mesh_factory
{
	void plane(Mesh & mesh, float sizeX, float sizeY)
	{
		const float halfSizeX = sizeX * 0.5f;
		const float halfSizeY = sizeY * 0.5f;

		const std::vector<MeshVertex> vertices = {
			{ Vector3(-halfSizeX, -halfSizeY, 0.0f), Vector3::posY, Vector4::one, Vector2(1, 0) },
			{ Vector3(halfSizeX, -halfSizeY, 0.0f), Vector3::posY, Vector4::one, Vector2(0, 0) },
			{ Vector3(halfSizeX, halfSizeY, 0.0f), Vector3::posY, Vector4::one, Vector2(0, 1) },
			{ Vector3(-halfSizeX, halfSizeY, 0.0f), Vector3::posY, Vector4::one, Vector2(1, 1) }
		};

		const std::vector<uint32_t> indices = {
			0, 1, 2,
			2, 3, 0};

		mesh.setData(vertices, indices);
	}

	void plane(Mesh & mesh, float size)
	{
		plane(mesh, size, size);
	}

	void cube(Mesh & mesh, float sizeX, float sizeY, float sizeZ, bool uvMapAsNet)
	{
		const float halfSizeX = sizeX * 0.5f;
		const float halfSizeY = sizeY * 0.5f;
		const float halfSizeZ = sizeZ * 0.5f;

		const Vector3 leftBottomBack(-halfSizeX, -halfSizeY, -halfSizeZ);
		const Vector3 leftBottomFront(-halfSizeX, -halfSizeY, halfSizeZ);
		const Vector3 leftTopBack(-halfSizeX, halfSizeY, -halfSizeZ);
		const Vector3 leftTopFront(-halfSizeX, halfSizeY, halfSizeZ);
		const Vector3 rightBottomBack(halfSizeX, -halfSizeY, -halfSizeZ);
		const Vector3 rightBottomFront(halfSizeX, -halfSizeY, halfSizeZ);
		const Vector3 rightTopBack(halfSizeX, halfSizeY, -halfSizeZ);
		const Vector3 rightTopFront(halfSizeX, halfSizeY, halfSizeZ);

		std::vector<MeshVertex> vertices;

		if (uvMapAsNet)
		{
			const float u = 1.0f / 4.0f;
			const float v = 1.0f / 3.0f;
			vertices = {
				{ leftBottomBack, Vector3::negX, Vector4::one, Vector2(u * 2, v * 1) }, // Left face (-x)
				{ leftBottomFront, Vector3::negX, Vector4::one, Vector2(u * 1, v * 1) },
				{ leftTopBack, Vector3::negX, Vector4::one, Vector2(u * 2, 0) },
				{ leftTopFront, Vector3::negX, Vector4::one, Vector2(u * 1, 0) },

				{ rightBottomBack, Vector3::posX, Vector4::one, Vector2(u * 2, v * 2) }, // Right face (+x)
				{ rightBottomFront, Vector3::posX, Vector4::one, Vector2(u * 1, v * 2) },
				{ rightTopBack, Vector3::posX, Vector4::one, Vector2(u * 2, 1) },
				{ rightTopFront, Vector3::posX, Vector4::one, Vector2(u * 1, 1) },

				{ leftBottomBack, Vector3::negY, Vector4::one, Vector2(u * 2, v * 1) }, // Bottom face (-y)
				{ leftBottomFront, Vector3::negY, Vector4::one, Vector2(u * 1, v * 1) },
				{ rightBottomBack, Vector3::negY, Vector4::one, Vector2(u * 2, v * 2) },
				{ rightBottomFront, Vector3::negY, Vector4::one, Vector2(u * 1, v * 2) },

				{ leftTopBack, Vector3::posY, Vector4::one, Vector2(u * 3, v * 1) }, // Top face (+y)
				{ leftTopFront, Vector3::posY, Vector4::one, Vector2(1, v * 1) },
				{ rightTopBack, Vector3::posY, Vector4::one, Vector2(u * 3, v * 2) },
				{ rightTopFront, Vector3::posY, Vector4::one, Vector2(1, v * 2) },

				{ leftBottomBack, Vector3::negZ, Vector4::one, Vector2(u * 2, v * 1) }, // Back face (-z)
				{ leftTopBack, Vector3::negZ, Vector4::one, Vector2(u * 3, v * 1) },
				{ rightBottomBack, Vector3::negZ, Vector4::one, Vector2(u * 2, v * 2) },
				{ rightTopBack, Vector3::negZ, Vector4::one, Vector2(u * 3, v * 2) },

				{ leftBottomFront, Vector3::posZ, Vector4::one, Vector2(u * 1, v * 1) }, // Front face (+z)
				{ leftTopFront, Vector3::posZ, Vector4::one, Vector2(0, v * 1) },
				{ rightBottomFront, Vector3::posZ, Vector4::one, Vector2(u * 1, v * 2) },
				{ rightTopFront, Vector3::posZ, Vector4::one, Vector2(0, v * 2) }
			};
		}
		else
		{
			vertices = {
				{leftBottomBack, Vector3::negX, Vector4::one, Vector2(0, 1)}, // Left face (-x)
				{leftBottomFront, Vector3::negX, Vector4::one, Vector2(1, 1)},
				{leftTopBack, Vector3::negX, Vector4::one, Vector2(0, 0)},
				{leftTopFront, Vector3::negX, Vector4::one, Vector2(1, 0)},

				{rightBottomBack, Vector3::posX, Vector4::one, Vector2(1, 1)}, // Right face (+x)
				{rightBottomFront, Vector3::posX, Vector4::one, Vector2(0, 1)},
				{rightTopBack, Vector3::posX, Vector4::one, Vector2(0, 0)},
				{rightTopFront, Vector3::posX, Vector4::one, Vector2(1, 0)},

				{leftBottomBack, Vector3::negY, Vector4::one, Vector2(0, 1)}, // Bottom face (-y)
				{leftBottomFront, Vector3::negY, Vector4::one, Vector2(0, 0)},
				{rightBottomBack, Vector3::negY, Vector4::one, Vector2(1, 1)},
				{rightBottomFront, Vector3::negY, Vector4::one, Vector2(1, 0)},

				{leftTopBack, Vector3::posY, Vector4::one, Vector2(0, 0)}, // Top face (+y)
				{leftTopFront, Vector3::posY, Vector4::one, Vector2(0, 1)},
				{rightTopBack, Vector3::posY, Vector4::one, Vector2(1, 0)},
				{rightTopFront, Vector3::posY, Vector4::one, Vector2(1, 1)},

				{leftBottomBack, Vector3::negZ, Vector4::one, Vector2(1, 1)}, // Back face (-z)
				{leftTopBack, Vector3::negZ, Vector4::one, Vector2(1, 0)},
				{rightBottomBack, Vector3::negZ, Vector4::one, Vector2(0, 1)},
				{rightTopBack, Vector3::negZ, Vector4::one, Vector2(0, 0)},

				{leftBottomFront, Vector3::posZ, Vector4::one, Vector2(0, 1)}, // Front face (+z)
				{leftTopFront, Vector3::posZ, Vector4::one, Vector2(0, 0)},
				{rightBottomFront, Vector3::posZ, Vector4::one, Vector2(1, 1)},
				{rightTopFront, Vector3::posZ, Vector4::one, Vector2(1, 0)}
			};
		}

		const std::vector<uint32_t> indices = {
			0, 1, 3, // Left face (-x)
			0, 3, 2,

			5, 4, 6, // Right face (+x)
			5, 6, 7,

			8, 10, 11, // Bottom face (-y)
			8, 11, 9,

			13, 15, 14, // Top face (+y)
			13, 14, 12,

			18, 16, 17, // Back face (-z)
			18, 17, 19,

			20, 22, 23, // Front face (+z)
			20, 23, 21 };

		mesh.setData(vertices, indices);
	}

	void cube(Mesh & mesh, float size, bool uvMapAsNet)
	{
		cube(mesh, size, size, size, uvMapAsNet);
	}

	void sphere(Mesh & mesh, float radius)
	{

	}

	void cone(Mesh & mesh, float radius, float height)
	{

	}

	void cylinder(Mesh & mesh, float radius, float height)
	{

	}
}