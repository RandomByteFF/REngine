#pragma once
// Everything that changes depending on the current frame in flight

#include "commandBuffer.hpp"
#include <memory>

namespace REngine::Core {
	class RenderStore {
		inline static std::weak_ptr<RenderStore> Instance;
	public:
		CommandBuffer cb;
		RenderStore& operator=(const RenderStore &) = delete;
		static void SetInstance(std::weak_ptr<RenderStore> i) {
			Instance = i;
		}

		static std::weak_ptr<RenderStore> Get() {
			return Instance;
		}

	};
}