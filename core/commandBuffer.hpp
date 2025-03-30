#pragma once

#include "headers.h"

namespace REngine::Core {
	class CommandBuffer {
		vk::RenderPass renderPass;
		vk::CommandBuffer commandBuffer;
		
	public:
		void Create();
		void Reset();
		void Begin();
		void BeginPass(vk::RenderPass renderPass, vk::Extent2D extent, vk::Framebuffer frameBuffer);
		void End();
		void EndPass();

		vk::CommandBuffer &GetBuffer();
		
		static vk::CommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(vk::CommandBuffer commandBuffer);
	};
}