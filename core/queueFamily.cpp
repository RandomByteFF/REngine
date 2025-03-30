#include "queueFamily.hpp"

namespace REngine::Core {

	QueueFamilyIndices QueueFamilyIndices::FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		QueueFamilyIndices indices;

		int i = 0;
		for (const auto& queueFamily : device.getQueueFamilyProperties()) {
			uint32_t presentSupport = device.getSurfaceSupportKHR(i, surface);
			if (presentSupport) {
				indices.presentFamily = i;
			}
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}
			i++;
		}

		return indices;
	}
}