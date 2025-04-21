#pragma once

#include "core/swapchain.hpp"
#include "core/commandBuffer.hpp"
#include "grid.hpp"

namespace REngine::Editor {
	class Editor {
		vk::RenderPass renderPass;
		std::vector<vk::Framebuffer> framebuffers;
		std::vector<vk::DescriptorSet> renderedViewports;
		std::shared_ptr<Grid> grid;

	public:
		void Initialize(Core::Swapchain swapchain, vk::RenderPass vpRenderPass, std::vector<std::shared_ptr<Core::Drawable>> &objects);
		void CreateFramebuffers(Core::Swapchain swapchain);
		void AddTextures(std::vector<vk::ImageView> &views, vk::Sampler sampler);
		void Render(uint32_t imageIndex, Core::CommandBuffer cb, vk::Extent2D extent);

		void DestroyBuffers();
		void Destroy();
	};
}