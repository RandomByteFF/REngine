#pragma once

#include "core/swapchain.hpp"
#include "core/commandBuffer.hpp"
#include "sceneTree.hpp"
#include "grid.hpp"
#include <memory>
#include "imgui.h"
#include "inspector.hpp"
#include "serializer.hpp"

namespace REngine::Editor {
	class Editor {
		ImVec2 prevViewSize = {0, 0};

		vk::RenderPass renderPass;
		std::vector<vk::Framebuffer> framebuffers;
		std::vector<vk::DescriptorSet> renderedViewports;
		std::shared_ptr<Grid> grid;
		SceneTree sceneTree;
		Inspector inspector;
		Serializer serializer;

	public:
		void Initialize(Core::Swapchain swapchain, vk::RenderPass vpRenderPass);
		void CreateFramebuffers(Core::Swapchain swapchain);
		void AddTextures(std::vector<vk::ImageView> &views, vk::Sampler sampler);
		void Render(uint32_t imageIndex, Core::CommandBuffer cb, vk::Extent2D extent);

		void DestroyBuffers();
		void Destroy();
	};
}