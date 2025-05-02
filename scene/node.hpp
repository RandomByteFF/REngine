#pragma once
#include <memory>
#include <vector>
#include "node.hpp"

namespace REngine::Scene {
	class Node {
		std::vector<std::shared_ptr<Node>> children;
		Node *parent;
	public:
		virtual void Update();
		void AddChild(std::shared_ptr<Node> child);
		virtual void Initialize() {};
		virtual void Destroy();
	};
}