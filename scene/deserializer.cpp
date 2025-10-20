#include "deserializer.hpp"

#include "mesh.hpp"
#include <memory>
#include <stdexcept>
#include <iostream>

namespace REngine::Scene {
	void Deserializer::loadMesh(const toml::table &tbl, std::shared_ptr<Mesh> node) {
		loadNode3D(tbl, node);
	}

	void Deserializer::loadNode3D(const toml::table &tbl, std::shared_ptr<Node3D> node) {
		loadNode(tbl, node);
		toml::array pos = *tbl["position"].as_array();
		toml::array rot = *tbl["rotation"].as_array();
		toml::array scale = *tbl["scale"].as_array();
		node->Position(glm::vec3(*pos[0].value<float>(), *pos[1].value<float>(), *pos[2].value<float>()));
		node->Scale(glm::vec3(*scale[0].value<float>(), *scale[1].value<float>(), *scale[2].value<float>()));
		node->RotationQuat(glm::quat(*rot[0].value<float>(), *rot[1].value<float>(), *rot[2].value<float>(), *rot[3].value<float>()));
	}

	void Deserializer::loadNode(const toml::table &tbl, std::shared_ptr <Node> node) {
		// node->id = tbl["id"].value<int64_t>().value();
		node->name = tbl["name"].value<std::string>().value();
	}

	std::shared_ptr<SceneTree> Deserializer::loadTree(const std::string sceneName) {
		toml::table tbl;
		try {
			tbl = toml::parse_file(sceneName);
		}
		catch (const toml::parse_error&) {
			// TODO: something more elegant
			std::cerr << "Scene not found!";
			return nullptr;
		}
		// std::shared_ptr<SceneTree> tree = std::shared_ptr<SceneTree>(new SceneTree());

		toml::array names = *tbl["nodes"].as_array();
		std::vector<std::shared_ptr<Node>> storage;
		for (size_t i = 0; i < names.size(); i++) {
			toml::table t = *tbl[*names[i].value<std::string>()].as_table();
			std::string type = t["type"].value<std::string>().value();
			int64_t parent = t["parent"].value<int64_t>().value();
		
			uint32_t id = t["id"].value<int64_t>().value();
			std::shared_ptr<Node> node;
			if (id == 0) {
				node = SceneTree::Current()->GetRoot();
			}
			else {
				node = SceneTree::Current()->Find(id, *SceneTree::Current()->GetRoot());
			}
			if (!node) throw std::runtime_error(std::format("Can't find node with ID: {}", id));
			if (type == "Mesh") {
				loadMesh(t, std::dynamic_pointer_cast<Mesh>(node));
			} else if (type == "Node3D") {
				loadNode3D(t, std::dynamic_pointer_cast<Node3D>(node));
			} else if (type == "Node") {
				loadNode(t, node);
			}

			// std::shared_ptr<Node> sharedNode(node);
			// if (int64_t(storage.size()) <= sharedNode->id) {
			// 	storage.resize(sharedNode->id + 1);
			// }

			// storage[sharedNode->id] = sharedNode;
			
			// if (sharedNode->id == 0) {
			// 	// This is the root
			// 	tree->SetRoot(sharedNode);
			// } else {
			// 	storage[parent]->AddChild(sharedNode);
			// }
		}
		
		return nullptr;
	}
}