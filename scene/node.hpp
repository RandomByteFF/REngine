#pragma once
#include <memory>
#include <vector>
#include <string>
#include "node.hpp"
#include "editor/inspectorVisitor.hpp"

#define VISITOR(parentType) \
	virtual void AcceptGui(Editor::InspectorVisitor &visitor) override { \
		visitor.Visit(this); \
		parentType::AcceptGui(visitor); \
	}

namespace REngine::Scene {
	class Node {
		std::vector<std::shared_ptr<Node>> children;
		Node *parent;
	public:
		std::string name = "";
		virtual void Update();
		void AddChild(std::shared_ptr<Node> child);
		//TODO: remove child
		virtual void Destroy();

		const std::vector<std::shared_ptr<Node>> &Children() const;

		virtual void AcceptGui(Editor::InspectorVisitor &visitor) {
			visitor.Visit(this);
		}
	};
}