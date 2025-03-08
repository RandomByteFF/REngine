#pragma once

#include <vulkan/vulkan.hpp>

namespace REngine::Core {
	class Image {
	public:
		
		void Image::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, 
			VkSampleCountFlagBits numSample, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
	};
}