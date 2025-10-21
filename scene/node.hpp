#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "common/nodeVisitor.hpp"
#include <iterator>
#include <functional>

#define VISITOR(parentType) \
	virtual void AcceptGui(Common::NodeVisitor *visitor) override { \
		visitor->Visit(this); \
		parentType::AcceptGui(visitor); \
	}

	
namespace REngine::Scene {
	class SceneTree;
	
	class Node {
		Node *parent = nullptr;
		glm::mat4 identity = glm::mat4(1.f);
		
	protected:
		std::vector<std::shared_ptr<Node>> children;
		virtual void EnteredTree();

	public:
		SceneTree *sceneTree;
		bool editorOnly = false;
		int64_t id = 0;
		std::string name = "";
		bool visible = true;
		virtual void Update();
		virtual void ApplyTransforms();

		virtual const glm::mat4 &GetModel() { return identity; }

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