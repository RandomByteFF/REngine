#include "node.hpp"
#include "sceneTree.hpp"

namespace REngine::Scene {
	void Node::EnteredTree() {
		//TODO: This doesn't get called on root otherwise ID would get bumped one
		sceneTree = parent->sceneTree;
		sceneTree->EnterTree(this);
	}

	void Node::Update() {
		for (auto i : children) i->Update();
	}

	void Node::ApplyTransforms() {
		auto mat = glm::mat4(1.f);
		for(auto i : children) {
			i->ApplyTransforms();
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

	void Node::Traverse(const std::function<void(Node *)> &lambda){
		lambda(this);
		for (auto i : children) i->Traverse(lambda);
	}
}
