#include "swapchain.hpp"

#include <limits>

#include "instance.hpp"
#include "image.hpp"

namespace REngine::Core {
	SwapchainSupportDetails Swapchain::QuerySwapchainSupport(vk::PhysicalDevice device) {
		SwapchainSupportDetails details;
		vk::SurfaceKHR surface = Instance::GetInfo().surface;
		details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
		details.formats = device.getSurfaceFormatsKHR(surface);
		details.presentModes = device.getSurfacePresentModesKHR(surface);
		return details;
	}

	void Swapchain::CreateSwapchain() {
		Info info = Instance::GetInfo();
		SwapchainSupportDetails swapChainSupport = QuerySwapchainSupport(info.physicalDevice);

		vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR createInfo {};
		createInfo.surface = Instance::GetInfo().surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		QueueFamilyIndices indices = Instance::GetInfo().queues;
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = true;
		createInfo.oldSwapchain = nullptr;

		swapchain = info.device.createSwapchainKHR(createInfo);
	
		images = info.device.getSwapchainImagesKHR(swapchain);
		imageFormat = surfaceFormat.format;
		this->extent = extent;


		imageViews.resize(images.size());
		for (size_t i = 0; i < images.size(); i++) {
			imageViews[i] = Image::CreateImageView(images[i], imageFormat);
		}
	}

	vk::Extent2D Swapchain::Extent() const {
		return extent;
	}

	VkFormat Swapchain::ImageFormat() const {
		return VkFormat(imageFormat);
	}

	const std::vector<vk::ImageView> &Swapchain::Views() const {
		return imageViews;
	}

	vk::SurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
		for (const auto &availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	
	vk::PresentModeKHR Swapchain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
		for (const auto &availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
		}
		
		return vk::PresentModeKHR::eFifo;
	}
	
	vk::Extent2D Swapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			Info info = Instance::GetInfo();
			int width, height;

			VkExtent2D actualExtent = {
				uint32_t(info.fbWidth),
				uint32_t(info.fbHeight)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.maxImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}