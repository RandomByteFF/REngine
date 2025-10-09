#pragma once

#include "core/IViews.hpp"
#include "core/swapchain.hpp"
#include "core/commandBuffer.hpp"
#include "sceneTree.hpp"
#include "grid.hpp"
#include <memory>
#include "imgui.h"
#include "inspector.hpp"
#include "serializer.hpp"
#include "core/renderPass.hpp"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace REngine::Editor {
	class Editor {
		ImVec2 prevViewSize = {0, 0};

		Core::RenderPass renderPass;
		Core::RenderPass editorViewRP;
		vk::ImageMemoryBarrier barrier;
		std::weak_ptr<Core::IViews> vpViews;
		std::vector<vk::DescriptorSet> renderedViewports;
		std::vector<vk::DescriptorSet> renderedEditorViews;
		std::shared_ptr<Grid> grid;
		SceneTree sceneTree;
		Inspector inspector;
		Serializer serializer;
		vk::Sampler sampler;


	public:
		void Initialize(std::shared_ptr<Core::Swapchain> swapchain, Core::RenderPass vpRenderPass);
		void AddTextures(vk::Sampler sampler);
		void Render(uint32_t imageIndex, Core::CommandBuffer cb, vk::Extent2D extent);

		void Recreate();
		void Destroy();
	};
}