#pragma once
#include "toml.hpp"
#include "common/nodeVisitor.hpp"
#include "scene/sceneTree.hpp"

namespace REngine::Editor {
	class Serializer : public Common::NodeVisitor {
		toml::table currentNode;
		std::string currentNodeType = "";
	public:
		void SerializeTree(Scene::SceneTree &tree);
		VISIT()
	};
}