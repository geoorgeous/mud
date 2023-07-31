#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP

#include "lights.hpp"
#include "material.hpp"
#include "math/matrix.hpp"
#include "mesh.hpp"
#include "utils/asset_object.hpp"
#include "utils/node_tree.hpp"

namespace mud
{
	struct SceneGraphNodeData
	{
		SceneGraphNodeData();

		bool isHidden;
		Matrix4 transform;
		std::vector<std::pair<Asset<Material> *, Asset<Mesh> *>> materialMeshPairs;
		std::vector<PointLight> pointLights;
	};

	class SceneGraph : public AssetObject<SceneGraph>, public NodeTree<SceneGraphNodeData>
	{
	public:

		virtual bool deserialize(std::ifstream & file) override;

		virtual bool serialize(std::ofstream & file) const override;

		static Matrix4 getNodeWorldTransform(const Node & node);
	};

	template<>
	inline AssetObject<SceneGraph>::AssetObject()
		: AssetObjectBase(AssetObjectType::SceneGraph)
	{ }
}

#endif