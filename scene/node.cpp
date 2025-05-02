#include "node.hpp"

namespace REngine::Scene {
	void Node::Update() {
		for (auto i : children) i->Update();
	}
	
	void Node::AddChild(std::shared_ptr<Node> child) {
		children.push_back(child);
		child->parent = this;
		child->Initialize();
	}
	void Node::Destroy() {
		for (auto i : children) i->Destroy();
	}
}
