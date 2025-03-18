#pragma once

#include "headers.h"

//TODO: move this to commandBuffer
namespace REngine::Core {
	class Queue {
	public:
		static vk::CommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
	};
}