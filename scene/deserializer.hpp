#pragma once

#include <memory>
#include "sceneTree.hpp"
#include "toml.hpp"

namespace REngine::Scene {
	class Deserializer {
		static void loadMesh(const toml::table &tbl, std::shared_ptr<Mesh> node);
		static void loadNode3D(const toml::table &tbl, std::shared_ptr<Node3D> node);
		static void loadNode(const toml::table &tbl, std::shared_ptr<Node> node);
	public:
		static std::shared_ptr<SceneTree> loadTree(const std::string sceneName);
	};
}