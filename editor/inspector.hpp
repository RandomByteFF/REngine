#pragma once

#include <optional>
#include <memory>
#include "scene/node.hpp"
#include "inspectorVisitor.hpp"

namespace REngine::Editor {
	class Inspector {
		InspectorVisitor visitor;
	public:
		void Gui(std::optional<std::shared_ptr<Scene::Node>> target);
	};
}