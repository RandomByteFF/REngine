#pragma once
#include "headers.h"
#include "swapchain.hpp"
#include "image.hpp"
#include "object.hpp"
#include "commandBuffer.hpp"
#include "windowManager.hpp"

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
		void CreateRenderPass();
		void CreateFrameBuffers();
		void CreateSyncObjects();
		void CreateSwapchain();
		void CleanupSwapchain();
		
		uint32_t currentFrame = 0;
		public:
		Swapchain swapchain;
		void Create(WindowManager window);
		const Swapchain Swapchain() const;
		const vk::RenderPass RenderPass() const;
		void Render(std::vector<Object> objects, Buffer vertexBuffer, Buffer indexBuffer, uint32_t indicesSize);
		void RecreateSwapchain();

		void Destroy();

		static vk::Format FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	};
}