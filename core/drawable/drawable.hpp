#pragma once
#include "core/headers.h"
#include "core/camera.hpp"

namespace REngine::Core {
	class Drawable {
	public:
		virtual void Bind(vk::CommandBuffer cb) = 0;
		virtual void Draw(vk::CommandBuffer cb) = 0;
		virtual void Update(Core::Camera &camera) = 0;
		virtual void Destroy() = 0;
	};
}