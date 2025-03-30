#pragma once

#include "headers.h"
#include "swapchain.hpp"
#include "image.hpp"
#include "drawable/mesh.hpp"
#include "commandBuffer.hpp"
#include "windowManager.hpp"
#include "camera.hpp"

namespace REngine::Core {
	class Renderer {
		WindowManager window;
		vk::Device device;
		vk::Format colorFormat;
		vk::Format depthFormat;
		Image depthImage;
		Image colorImage;
		vk::RenderPass renderPass;
		std::vector<vk::Fence> inFlightFences;
		std::vector<vk::Semaphore> imageAvailableSemaphores;
		std::vector<vk::Semaphore> renderFinishedSemaphores;
		std::vector<vk::Framebuffer> swapChainFrameBuffers;
		std::vector<CommandBuffer> commandBuffers;
		vk::Sampler sampler;
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateSyncObjects();
		void CreateImages();
		void CleanupSwapchain();
		void CreateSampler();
		
		uint32_t currentFrame = 0;

	public:
		Swapchain swapchain;
		void Create(WindowManager window);
		const Swapchain GetSwapchain() const;
		const vk::RenderPass RenderPass() const;
		const vk::Sampler Sampler() const;
		void Render(std::vector<Mesh> &objects, Camera &camera);
		void RecreateSwapchain();

		void Destroy();

		static vk::Format FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	};
}