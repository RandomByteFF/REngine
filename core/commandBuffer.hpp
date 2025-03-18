#pragma once

#include "headers.h"

namespace REngine::Core {
	class CommandBuffer {
		vk::RenderPass renderPass;
		public:
		vk::CommandBuffer commandBuffer;
		void Create(vk::RenderPass renderPass);
		void Reset();
		void BeginPass(vk::Extent2D extent, vk::Framebuffer frameBuffer);
		void End();
	};
}