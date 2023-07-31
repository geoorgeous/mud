#include "scene_graph.hpp"

#include "utils/asset_manager.hpp"
#include "utils/logger.hpp"

namespace mud
{
	bool deserializeNode(std::ifstream & file, SceneGraph & graph, SceneGraph::Node * node)
	{
		serialization_helpers::deserialize(file, node->data.transform);

		size_t numPairs = 0;
		serialization_helpers::deserialize(file, numPairs);

		for (size_t idx = 0; idx < numPairs; ++idx)
		{
			Asset<Material> * materialAsset = AssetManager::getInstance().deserializeAssetReference<Material>(file);
			if (materialAsset == nullptr || !materialAsset->load())
			{
				log(LogLevel::Error, fmt::format("Failed to deserialize scene graph: failed to deserialize node material\n"));
				return false;
			}

			Asset<Mesh> * meshAsset = AssetManager::getInstance().deserializeAssetReference<Mesh>(file);
			if (meshAsset == nullptr || !meshAsset->load())
			{
				log(LogLevel::Error, fmt::format("Failed to deserialize scene graph: failed to deserialize node mesh\n"));
				return false;
			}

			node->data.materialMeshPairs.emplace_back(materialAsset, meshAsset);
		}
		
		serialization_helpers::deserializeVector(file, node->data.pointLights);

		size_t numChildren = 0;
		serialization_helpers::deserialize(file, numChildren);

		//log(LogLevel::Trace, fmt::format("Deserializing scene graph node: {0} node meshes, {1} children\n", numPairs, numChildren));

		for (size_t idx = 0; idx < numChildren; ++idx)
		{
			SceneGraph::Node * newChildNode = graph.newNode();
			if (!deserializeNode(file, graph, newChildNode))
				return false;
			graph.setNodeParent(newChildNode, node);
		}

		return true;
	}
	
	SceneGraphNodeData::SceneGraphNodeData()
		: isHidden(false)
	{}

	bool SceneGraph::deserialize(std::ifstream & file)
	{
		size_t numRootNodes = 0;
		serialization_helpers::deserialize(file, numRootNodes);

		for (size_t idx = 0; idx < numRootNodes; ++idx)
		{
			Node * newRootNode = newNode();
			if (!deserializeNode(file, *this, newRootNode))
				return false;
		}

		return true;
	}

	void serializeNode(std::ofstream & file, const SceneGraph::Node * node)
	{
		serialization_helpers::serialize(file, node->data.transform);
		serialization_helpers::serialize(file, node->data.materialMeshPairs.size());

		for (const auto & pair : node->data.materialMeshPairs)
		{
			pair.first->serializeReference(file);
			pair.second->serializeReference(file);
		}

		serialization_helpers::serializeVector(file, node->data.pointLights);

		serialization_helpers::serialize(file, node->children.size());

		for (size_t idx = 0; idx < node->children.size(); ++idx)
			serializeNode(file, node->children[idx]);
	}

	bool SceneGraph::serialize(std::ofstream & file) const
	{
		const std::vector<Node *> & rootNodes = getRootNodes();

		serialization_helpers::serialize(file, rootNodes.size());

		for (const auto & rootNode : rootNodes)
			serializeNode(file, rootNode);

		return true;
	}

	Matrix4 SceneGraph::getNodeWorldTransform(const SceneGraph::Node & node)
	{
		Matrix4 worldTransform = node.data.transform;
		const SceneGraph::Node * parent = node.parent;

		while (parent != nullptr)
		{
			worldTransform = parent->data.transform * worldTransform;
			parent = parent->parent;
		}

		return worldTransform;
	}
}