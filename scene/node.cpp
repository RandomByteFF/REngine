#include "node.hpp"

namespace REngine::Scene {
	void Node::Update() {
		for (auto i : children) i->Update();
	}
	
	void Node::AddChild(std::shared_ptr<Node> child) {
		children.push_back(child);
		child->parent = this;
	}
	void Node::Destroy() {
		for (auto i : children) i->Destroy();
	}
	
	const std::vector<std::shared_ptr<Node>> &Node::Children() const {
		return children;
	}
}
