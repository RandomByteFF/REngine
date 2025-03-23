#pragma once

#include "headers.h"

namespace REngine::Core {
	class CommandBuffer {
		vk::RenderPass renderPass;
		vk::CommandBuffer commandBuffer;
		
	public:
		void Create(vk::RenderPass renderPass);
		void Reset();
		void BeginPass(vk::Extent2D extent, vk::Framebuffer frameBuffer);
		void End();

		vk::CommandBuffer &GetBuffer();
		
		static vk::CommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
	};
}