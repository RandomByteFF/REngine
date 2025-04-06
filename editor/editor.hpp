#pragma once

#include "core/swapchain.hpp"
#include "core/commandBuffer.hpp"

namespace REngine::Editor {
	class Editor {
		vk::RenderPass renderPass;
		std::vector<vk::Framebuffer> framebuffers;
		std::vector<vk::DescriptorSet> renderedViewports;
	public:
		void Initialize(Core::Swapchain swapchain);
		void CreateFramebuffers(Core::Swapchain swapchain);
		void AddTextures(std::vector<vk::ImageView> &views, vk::Sampler sampler);
		void Render(uint32_t imageIndex, Core::CommandBuffer cb, vk::Extent2D extent);

		void DestroyBuffers();
		void Destroy();
	};
}