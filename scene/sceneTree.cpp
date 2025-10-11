#include "sceneTree.hpp"
#include <cassert>
#include "node.hpp"

namespace REngine::Scene {
	SceneTree *SceneTree::Current() {
		assert(current != nullptr);
		return current;
	}

	void SceneTree::SetCurrent() {
		current = this;
	}
	std::shared_ptr<Core::Camera> SceneTree::ActiveCamera() {
		return activeCamera;
	}

	void SceneTree::SetActiveCamera(std::shared_ptr<Core::Camera> camera) {
		activeCamera = camera;
	}

	std::list<Drawable*>::iterator SceneTree::AddToDrawList(Drawable *drawable, uint8_t order) {
		drawList[order].push_back(drawable);
		return --drawList[order].end();
	}
	
	//TODO: test
	void SceneTree::RemoveFromDrawList(Drawable *drawable) {
		for(auto i = drawList.begin(); i != drawList.end(); i++) {
			for(auto j = i->begin(); j != i->end(); j++) {
				if (*j == drawable) {
					i->erase(j);
					return;
				}
			}
		}
	}

	void SceneTree::RemoveFromDrawList(std::list<Drawable *>::iterator iterator, uint8_t order) {
		drawList[order].erase(iterator);
	}

	void SceneTree::EnterTree(Node *node) {
		node->id = lastFreeId++;
	}

	void SceneTree::Draw(vk::CommandBuffer &cb) {
		for(auto i = drawList.begin(); i != drawList.end(); i++) {
			for(auto j = i->begin(); j != i->end(); j++) {
				(*j)->Draw(cb);
				//TODO: switch this out for: CallDrawlist
			}
		}
	}

	void SceneTree::PreDraw(Core::CommandBuffer &cb) {
		for(auto i = drawList.begin(); i != drawList.end(); i++) {
			for(auto j = i->begin(); j != i->end(); j++) {
				(*j)->PreDraw(cb);
				//FIXME: call this funcction on every node, not just drawable
			}
		}
	}

	void SceneTree::PostDraw(Core::CommandBuffer &cb) {
		for(auto i = drawList.begin(); i != drawList.end(); i++) {
			for(auto j = i->begin(); j != i->end(); j++) {
				(*j)->PostDraw(cb);
			}
		}
	}
	void SceneTree::CallDrawlist(std::function<void (Drawable &)> function) {
		for(auto i = drawList.begin(); i != drawList.end(); i++) {
			for(auto j = i->begin(); j != i->end(); j++) {
				function(**j);
			}
		}
	}

	void SceneTree::Update() {
		root->Update();
	}

	void SceneTree::Destroy() {
		root->Destroy();
	}
	
	void SceneTree::SetRoot(std::shared_ptr<Node> root) {
		this->root = root;
		root->id = lastFreeId++;
		root->sceneTree = this;
	}
	
	std::shared_ptr<Node> SceneTree::GetRoot() const {
		return root;
	}
}