#pragma once

#include "headers.h"

namespace REngine::Core {
	class Queue {
	public:
		static vk::CommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
	};
}