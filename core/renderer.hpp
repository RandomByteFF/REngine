#pragma once

#include "headers.h"
#include "renderPass.hpp"
#include "swapchain.hpp"
#include "commandBuffer.hpp"
#include "windowManager.hpp"
#include "camera.hpp"
#include "editor/editor.hpp"
#include "scene/sceneTree.hpp"

namespace REngine::Core {
	class Renderer {
		RenderPass vpRenderer;
		Editor::Editor editor;
		vk::Device device;
		vk::Format colorFormat;
		std::vector<vk::Fence> inFlightFences;
		std::vector<vk::Semaphore> imageAvailableSemaphores;
		std::vector<vk::Semaphore> renderFinishedSemaphores;
		std::vector<CommandBuffer> commandBuffers;
		vk::Sampler sampler;
		vk::ImageMemoryBarrier barrier;
		void CreateSyncObjects();
		void CleanupSwapchain();
		void CreateSampler();
		
		uint32_t currentFrame = 0;

	public:
		std::shared_ptr<Swapchain> swapchain;
		//FIXME: objects from here
		void Create();
		const Swapchain GetSwapchain() const;
		const vk::RenderPass GetRenderPass() const;
		const vk::Sampler Sampler() const;
		void Render(Scene::SceneTree &sceneTree, Camera &camera);
		void RecreateSwapchain();
		float AspectRatio();

		void Destroy();
	};
}