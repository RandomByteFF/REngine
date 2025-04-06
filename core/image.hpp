#pragma once

#include "headers.h"
#include "loader/image.hpp"

namespace REngine::Core {
	class Image {
		vk::Image image;
		VmaAllocation alloc;
		VmaAllocationInfo allocInfo;
		vk::ImageView view;
		vk::Format format;
		uint32_t mipLevels;
		int width;
		int height;
		
	public:
		void CreateImage(uint32_t width, uint32_t height, int mipLevels,
			vk::SampleCountFlagBits numSample, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, 
			vk::ImageAspectFlagBits aspect = vk::ImageAspectFlagBits::eColor);
		void CreateImage(int width, int height, int mipLevels, vk::DeviceSize size, unsigned char *pixels);
		void CreateImage(const Loader::Image &image, int mipLevels = 0);

		void TransitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
		static vk::ImageView CreateImageView(vk::Image image, vk::Format format, uint32_t mipLevels = 1, 
			vk::ImageAspectFlagBits aspectFlags = vk::ImageAspectFlagBits::eColor);
		void GenerateMipmaps(uint32_t mipLevels = 0);

		vk::ImageView View() const;
		vk::Image Get() const;
		vk::Format Format() const;
		uint32_t Width() const;
		uint32_t Height() const;
		void Destroy();
	};
}