#pragma once
#include "scene/node.hpp"
#include <optional>

namespace REngine::Editor {
	class SceneTree {
		void TraverseNode(const std::shared_ptr<Scene::Node> node, int &id);
		std::optional<std::shared_ptr<Scene::Node>> selected;
		void GenericPopup();
	public:
		void Gui();
		std::optional<std::shared_ptr<Scene::Node>> GetSelected();
	};
}