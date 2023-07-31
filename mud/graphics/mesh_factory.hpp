#ifndef MESH_FACTORY_HPP
#define MESH_FACTORY_HPP

#include <utility>
#include <string>
#include <vector>

#include "material.hpp"
#include "mesh.hpp"
#include "texture.hpp"

namespace mud::mesh_factory
{
	void plane(Mesh & mesh, float sizeX, float sizeY);

	void plane(Mesh & mesh, float size = 1.0f);

	void cube(Mesh & mesh, float sizeX, float sizeY, float sizeZ, bool uvMapAsNet = false);

	void cube(Mesh & mesh, float size = 1.0f, bool uvMapAsNet = false);

	void sphere(Mesh & mesh, float radius = 0.5f);

	void cone(Mesh & mesh, float radius = 0.25f, float height = 1.0f);

	void cylinder(Mesh & mesh, float radius = 0.25f, float height = 1.0f);
}

#endif