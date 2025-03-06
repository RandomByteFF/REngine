#pragma once

#include <vulkan/vulkan.hpp>
#include <optional>

namespace REngine::Core {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
	
		bool IsComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}

		static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
	};

}
