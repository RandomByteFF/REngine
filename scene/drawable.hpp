#pragma once
#include "core/headers.h"
#include "core/camera.hpp"
#include "core/commandBuffer.hpp"
#include "node.hpp"
#include <list>

namespace REngine::Scene {
	class SceneTree;
	class Drawable {
		uint8_t drawOrder = 0;
		std::list<Drawable*>::iterator drawListPos;
		SceneTree *tree;

	public:
		virtual void Draw(vk::CommandBuffer cb) = 0;
		virtual void DrawFromView(vk::CommandBuffer cb, Core::Camera &camera) {};
		virtual void Destroy();
		virtual void Initialize(SceneTree *sceneTree);
		virtual void PreDraw(Core::CommandBuffer cb) {}
		virtual void PostDraw(Core::CommandBuffer cb) {}
		virtual void Recreate() {};
		void SetDrawOrder(uint8_t drawOrder);
	};
}