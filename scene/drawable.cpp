#include "drawable.hpp"
#include "sceneTree.hpp"

namespace REngine::Scene {
	void Drawable::Initialize(SceneTree *sceneTree) {
		// TODO:: dont refer to current, should have a pointer to it's scene tree
		tree = sceneTree;
		drawListPos = tree->AddToDrawList(this, drawOrder);
	}
	
	void Drawable::SetDrawOrder(uint8_t drawOrder) {
		this->drawOrder = drawOrder;
	}

	void Drawable::Destroy() {
		tree->RemoveFromDrawList(drawListPos, drawOrder);
	}
}