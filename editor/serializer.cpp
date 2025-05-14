#include "serializer.hpp"
#include "scene/node3d.hpp"
#include <iostream>
#include <fstream>

namespace REngine::Editor {
	void Serializer::SerializeTree(Scene::SceneTree &tree) {
		toml::table serializedTree;
		toml::array nodes;

		tree.GetRoot()->Traverse([&](Scene::Node *node) {
			if (!node->editorOnly) {
				currentNodeType.clear();
				currentNode.clear();

				node->AcceptGui(this);

				std::string name = currentNodeType + std::to_string(node->id);
				nodes.push_back(name);
				serializedTree.insert(name, currentNode);
			}
		});
		serializedTree.insert("nodes", nodes);
		std::cout << serializedTree << std::endl;
		std::ofstream file("tree.rest");
		file << serializedTree;
		file.close();
	}
	void Serializer::Visit(Scene::Node *node) {
		if (currentNodeType.empty()) currentNodeType = "Node";
		currentNode.insert("type", currentNodeType);
		currentNode.insert("name", node->name);
		currentNode.insert("id", node->id);
		auto p = node->GetParent();
		int64_t pId = p == nullptr ? -1 : p->id;
		currentNode.insert("parent", pId);
	}

	void Serializer::Visit(Scene::Node3D *node) {
		if (currentNodeType.empty()) currentNodeType = "Node3D";
		auto pos = node->Position();
		currentNode.insert("position", toml::array {pos.x, pos.y, pos.z});
		auto scale = node->Scale();
		currentNode.insert("scale", toml::array {scale.x, scale.y, scale.z});
		auto rot = node->RotationQuat();
		currentNode.insert("rotation", toml::array {rot.x, rot.y, rot.z, rot.w});
	}
	
	void Serializer::Visit(Scene::Mesh *node) {
		if (currentNodeType.empty()) currentNodeType = "Mesh";
	}
}