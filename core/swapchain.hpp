#pragma once

#include "headers.h"

namespace REngine::Core {
	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	class Swapchain {
		vk::Format imageFormat;
		vk::Extent2D extent;
		
		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);
		
		public:
		std::vector<vk::Image> images;
		vk::SwapchainKHR swapchain;
		static SwapchainSupportDetails QuerySwapchainSupport(vk::PhysicalDevice device);
		void CreateSwapchain();
		vk::Extent2D Extent() const;
		// TODO: switch this to vk::Format
		VkFormat ImageFormat() const;
	};
}
