#include "drawable.hpp"
#include "sceneTree.hpp"

namespace REngine::Scene {
	void Drawable::Initialize() {
		// TODO:: dont refer to current, should have a pointer to it's scene tree
		drawListPos = SceneTree::Current()->AddToDrawList(this, drawOrder);
	}
	
	void Drawable::SetDrawOrder(uint8_t drawOrder) {
		this->drawOrder = drawOrder;
	}

	void Drawable::Destroy() {
		SceneTree::Current()->RemoveFromDrawList(drawListPos, drawOrder);
	}
}