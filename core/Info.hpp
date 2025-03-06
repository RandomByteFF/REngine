#pragma once

#include <vulkan/vulkan.hpp>
#include "queueFamily.hpp"
#include "swapchain.hpp"

namespace REngine::Core {
	struct Info {
		vk::SampleCountFlagBits maxMsaa;
		QueueFamilyIndices queues;
		vk::SurfaceKHR surface;
		SwapChainSupportDetails swapchainSupport;
		vk::PhysicalDevice physicalDevice;
	};
}