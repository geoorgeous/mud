#ifndef NODE_TREE_HPP
#define NODE_TREE_HPP

#include <algorithm>

namespace mud
{
	template<typename T>
	class NodeTree
	{
	public:

		struct Node
		{
			Node * parent;
			std::vector<Node *> children;
			T data;
		};

		~NodeTree()
		{
			for (auto & node : m_nodes)
				delete node;
		}

		const std::vector<Node *> & getRootNodes() const
		{
			return m_rootNodes;
		}

		const std::vector<Node *> & getNodes() const
		{
			return m_nodes;
		}

		Node * newNode()
		{
			Node * newNode = new Node;
			newNode->parent = nullptr;

			m_rootNodes.push_back(newNode);
			m_nodes.push_back(newNode);

			return newNode;
		}

		Node * newNode(const T & data)
		{
			Node * newNode = newNode();
			newNode->data = data;
			return newNode;
		}

		void deleteNode(Node * node, bool deleteChildren = true)
		{
			if (deleteChildren)
			{
				for (size_t idx = 0; idx < node->children.size(); ++idx)
					deleteNode(node->children[idx], true);
			}
			else
			{
				for (size_t idx = 0; idx < node->children.size(); ++idx)
					setNodeParent(node->children[idx], nullptr);
			}

			if (node->parent != nullptr)
			{
				auto iter = std::find(node->parent->children.begin(), node->parent->children.end(), node);
				if (iter != node->parent->children.end())
					node->parent->children.erase(iter);
			}
			else
			{
				auto iter = std::find(m_rootNodes.begin(), m_rootNodes.end(), node);
				if (iter != m_rootNodes.end())
					m_rootNodes.erase(iter);
			}

			auto iter = std::find(m_nodes.begin(), m_nodes.end(), node);
			if (iter != m_nodes.end())
				m_nodes.erase(iter);

			delete node;
		}

		void setNodeParent(Node * node, Node * newParent)
		{
			if (node->parent == newParent)
				return;

			if (node->parent != nullptr)
			{
				auto iter = std::find(node->parent->children.begin(), node->parent->children.end(), node);
				if (iter != node->parent->children.end())
					node->parent->children.erase(iter);
			}

			node->parent = newParent;

			if (node->parent != nullptr)
			{
				auto iter = std::find(m_rootNodes.begin(), m_rootNodes.end(), node);
				if (iter != m_rootNodes.end())
					m_rootNodes.erase(iter);
				node->parent->children.push_back(node);
			}
			else
				m_rootNodes.push_back(node);
		}

		Node * copyNode(const Node * node)
		{
			Node * copy = newNode();
			for (Node * child : node->children)
				setNodeParent(copyNode(child), copy);
			copy->data = node->data;
			return copy;
		}

		Node * copyNodeTree(const NodeTree<T> & otherTree)
		{
			Node * treeCopyRootNode = newNode();
			for (const Node * rootNode : otherTree.m_rootNodes)
				setNodeParent(copyNode(rootNode), treeCopyRootNode);
			return treeCopyRootNode;
		}

	private:

		std::vector<Node *> m_rootNodes;
		std::vector<Node *> m_nodes;
	};
}

#endif