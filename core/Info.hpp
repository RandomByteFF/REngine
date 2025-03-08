#pragma once

#include "headers.h"
#include "queueFamily.hpp"
#include "swapchain.hpp"

namespace REngine::Core {
	struct Info {
		vk::SampleCountFlagBits maxMsaa;
		QueueFamilyIndices queues;
		vk::SurfaceKHR surface;
		SwapchainSupportDetails swapchainSupport;
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		int fbWidth;
		int fbHeight;
	};
}