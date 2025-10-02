#pragma once
#include <memory>
#include "core/camera.hpp"
// #include "node.hpp"
#include <list>
#include <array>
#include "core/commandBuffer.hpp"
#include "drawable.hpp"

class Node;

namespace REngine::Scene {
	class SceneTree {
		SceneTree(const SceneTree&) = delete;
		SceneTree &operator=(const SceneTree&) = delete;
		
		inline static SceneTree *current;
		std::shared_ptr<Core::Camera> activeCamera;
		std::shared_ptr<Node> root;
		
		std::array<std::list<Drawable*>, 256> drawList;
		int64_t lastFreeId = 0;
		
	public:
		SceneTree() = default;
		static SceneTree *Current();
		void SetCurrent();
		std::shared_ptr<Core::Camera> ActiveCamera();
		void SetActiveCamera(std::shared_ptr<Core::Camera> camera);

		std::list<Drawable*>::iterator AddToDrawList(Drawable *drawable, uint8_t order);
		void RemoveFromDrawList(Drawable *drawable);
		void RemoveFromDrawList(std::list<Drawable*>::iterator iterator, uint8_t order);
		void EnterTree(Node* node);

		void Draw(vk::CommandBuffer &cb);
		void PreDraw(Core::CommandBuffer &cb);
		void PostDraw(Core::CommandBuffer &cb);
		void CallDrawlist(std::function<void(Drawable &)>);
		void Update();
		void ApplyTransforms();
		void Destroy();

		//TODO: later i would probably need to be able to differentiate between node / node2d / node3d roots
		void SetRoot(std::shared_ptr<Node> root);
		std::shared_ptr<Node> GetRoot() const;
	};
}