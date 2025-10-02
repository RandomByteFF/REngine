#pragma once

#include "headers.h"
#include "queueFamily.hpp"
#include "swapchain.hpp"
#include "vk_mem_alloc.h"
#include "vulkan/vulkan_structs.hpp"

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
		vk::SurfaceFormatKHR imageFormat;
		vk::Format depthFormat;
		int fbWidth;
		int fbHeight;
		vk::Extent2D swapchainExtent;
		const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t swapchainSize;
		uint32_t currentFrame = 0;
		uint32_t currentFb = 0;
	};
}