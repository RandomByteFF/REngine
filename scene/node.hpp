#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "node.hpp"
#include "common/nodeVisitor.hpp"
#include <iterator>
#include <functional>

#define VISITOR(parentType) \
	virtual void AcceptGui(Common::NodeVisitor *visitor) override { \
		visitor->Visit(this); \
		parentType::AcceptGui(visitor); \
	}

namespace REngine::Scene {
	class Node {
		Node *parent = nullptr;
		glm::mat4 identity = glm::mat4(1.f);
		
	protected:
		std::vector<std::shared_ptr<Node>> children;
		virtual void EnteredTree();

	public:
		//TODO: keep track of which tree we're in
		bool editorOnly = false;
		int64_t id = 0;
		std::string name = "";
		virtual void Update();
		virtual void ApplyTransforms(glm::mat4 &parentMatrix);
		Node *GetParent();
		void AddChild(std::shared_ptr<Node> child);
		//TODO: remove child
		virtual void Destroy();

		const std::vector<std::shared_ptr<Node>> &Children() const;

		virtual void AcceptGui(Common::NodeVisitor *visitor) {
			visitor->Visit(this);
		}

		void Traverse(const std::function<void(Node*)> &lambda);
	};
}