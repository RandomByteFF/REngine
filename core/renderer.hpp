#pragma once

#include "headers.h"
#include "swapchain.hpp"
#include "image.hpp"
#include "drawable/mesh.hpp"
#include "commandBuffer.hpp"
#include "windowManager.hpp"
#include "camera.hpp"
#include "viewportRenderer.hpp"
#include "editor/editor.hpp"

namespace REngine::Core {
	class Renderer {
		WindowManager window;
		ViewportRenderer vpRenderer;
		Editor::Editor editor;
		vk::Device device;
		vk::Format colorFormat;
		vk::Format depthFormat;
		std::vector<vk::Fence> inFlightFences;
		std::vector<Image> viewportImages;
		std::vector<vk::Semaphore> imageAvailableSemaphores;
		std::vector<vk::Semaphore> renderFinishedSemaphores;
		std::vector<CommandBuffer> commandBuffers;
		vk::Sampler sampler;
		vk::ImageMemoryBarrier barrier;
		std::vector<vk::ImageView> viewportView;
		void CreateSyncObjects();
		void CreateImages();
		void CleanupSwapchain();
		void CreateSampler();
		
		uint32_t currentFrame = 0;

	public:
		Swapchain swapchain;
		//FIXME: objects from here
		void Create(WindowManager window, std::vector<std::shared_ptr<Drawable>> &objects);
		const Swapchain GetSwapchain() const;
		const vk::RenderPass RenderPass() const;
		const vk::Sampler Sampler() const;
		void Render(std::vector<std::shared_ptr<Drawable>> &objects, Camera &camera);
		void RecreateSwapchain();
		float AspectRatio();

		void Destroy();

		static vk::Format FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	};
}