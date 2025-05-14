#include "node.hpp"

namespace REngine::Scene {
	void Node::Update() {
		for (auto i : children) i->Update();
	}

	void Node::ApplyTransforms(glm::mat4 &parentMatrix) {
		auto mat = glm::mat4(1.f);
		for(auto i : children) {
			i->ApplyTransforms(mat);
		}
	}

	Node *Node::GetParent() {
		return parent;
	}

	void Node::AddChild(std::shared_ptr<Node> child) {
		children.push_back(child);
		child->parent = this;
		child->EnteredTree();
	}
	void Node::Destroy() {
		for (auto i : children) i->Destroy();
	}
	
	const std::vector<std::shared_ptr<Node>> &Node::Children() const {
		return children;
	}
}
