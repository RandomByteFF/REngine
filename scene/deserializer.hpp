#pragma once

#include <memory>
#include "sceneTree.hpp"
#include "toml.hpp"

namespace REngine::Scene {
	class Deserializer {
		static void loadMesh(const toml::table &tbl, Mesh *node);
		static void loadNode3D(const toml::table &tbl, Node3D *node);
		static void loadNode(const toml::table &tbl, Node *node);
	public:
		static std::shared_ptr<SceneTree> loadTree(const std::string sceneName);
	};
}