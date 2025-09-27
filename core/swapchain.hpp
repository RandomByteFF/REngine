#pragma once

#include "IViews.hpp"
#include "headers.h"

namespace REngine::Core {
	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	class Swapchain : public IViews {
		inline static Swapchain* instance = nullptr;
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
		vk::Format imageFormat;
		vk::Extent2D extent;
		
		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);
		
	public:
		static Swapchain *Instance();
		static SwapchainSupportDetails QuerySwapchainSupport(vk::PhysicalDevice device);
		void CreateSwapchain();
		vk::Extent2D Extent() const;
		vk::Format ImageFormat() const;
		vk::SwapchainKHR GetSwapchain() const;
		virtual const std::vector<vk::ImageView> &Views() const override;
		uint32_t SwapchainImageCount() const;

		void Destroy();
	};
}
