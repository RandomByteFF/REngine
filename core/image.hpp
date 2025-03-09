#pragma once

#include "headers.h"

namespace REngine::Core {
	class Image {
		VmaAllocation alloc;
		VmaAllocationInfo allocInfo;
		vk::ImageView view;
		vk::Format format;
		uint32_t mipLevels;
		public:
		vk::Image image;
		void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, 
			vk::SampleCountFlagBits numSample, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, 
			vk::ImageAspectFlagBits aspect = vk::ImageAspectFlagBits::eColor);
		
		void TransitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
		static vk::ImageView CreateImageView(vk::Image image, vk::Format format, uint32_t mipLevels = 1, 
			vk::ImageAspectFlagBits aspectFlags = vk::ImageAspectFlagBits::eColor);

		vk::ImageView View() const;
		void Destroy();
	};
}