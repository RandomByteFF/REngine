#include "drawable.hpp"
#include "sceneTree.hpp"

namespace REngine::Scene {
	void Drawable::Initialize() {
		Node::Initialize();
		drawListPos = SceneTree::Current()->AddToDrawList(this, drawOrder);
	}
	
	void Drawable::SetDrawOrder(uint8_t drawOrder) {
		this->drawOrder = drawOrder;
	}

	void Drawable::Destroy() {
		Node::Destroy();
		SceneTree::Current()->RemoveFromDrawList(drawListPos, drawOrder);
	}
}