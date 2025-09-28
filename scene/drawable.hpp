#pragma once
#include "core/headers.h"
#include "core/camera.hpp"
#include "node.hpp"

namespace REngine::Scene {
	class SceneTree;
	class Drawable {
		uint8_t drawOrder = 0;
		std::list<Drawable*>::iterator drawListPos;
		SceneTree *tree;

	public:
		virtual void Draw(vk::CommandBuffer cb) = 0;
		virtual void Destroy();
		virtual void Initialize(SceneTree *sceneTree);
		virtual void PreDraw(vk::CommandBuffer cb) {}
		virtual void PostDraw(vk::CommandBuffer cb) {}
		void SetDrawOrder(uint8_t drawOrder);
	};
}