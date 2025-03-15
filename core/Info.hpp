#pragma once

#include "headers.h"
#include "queueFamily.hpp"
#include "swapchain.hpp"
#include "vk_mem_alloc.h"

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
		VmaAllocator allocator;
		vk::CommandPool commandPool;
		int fbWidth;
		int fbHeight;
		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	};
}