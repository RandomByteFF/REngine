#pragma once
#include "core/headers.h"
#include "core/camera.hpp"
#include "node.hpp"

namespace REngine::Scene {
	class Drawable {
		uint8_t drawOrder = 0;
		std::list<Drawable*>::iterator drawListPos;

	public:
		virtual void Draw(vk::CommandBuffer cb) = 0;
		virtual void Destroy();
		virtual void Initialize();
		void SetDrawOrder(uint8_t drawOrder);
	};
}