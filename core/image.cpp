#include "image.hpp"
#include "instance.hpp"
#include "queue.hpp"

namespace REngine::Core {
	void Image::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, 
		vk::SampleCountFlagBits numSample, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage,
		vk::ImageAspectFlagBits aspect) {

		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = usage;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		imageInfo.samples = numSample;

		VmaAllocationCreateInfo allocCreateInfo {};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		VkImage img;
		VkImageCreateInfo ii = imageInfo;
		vmaCreateImage(Instance::GetInfo().allocator, &ii, &allocCreateInfo, &img, &alloc, &allocInfo);
		image = img;
		this->format = format;
		this->mipLevels = mipLevels;
		view = CreateImageView(image, format, mipLevels, aspect);
	}

	void Image::TransitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
		vk::CommandBuffer commandBuffer = Queue::BeginSingleTimeCommands();
		vk::ImageMemoryBarrier barrier{};
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = vk::AccessFlagBits::eNone;

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else {
			throw std::invalid_argument("Unsupported layout transition!");
		}

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), nullptr, nullptr, barrier);

		Queue::EndSingleTimeCommands(commandBuffer);
	}
	
	vk::ImageView Image::CreateImageView(vk::Image image, vk::Format format, uint32_t mipLevels, vk::ImageAspectFlagBits aspectFlags) {
		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.image = image;
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		imageView = Instance::GetInfo().device.createImageView(viewInfo);
		return imageView;
	}

	vk::ImageView Image::View() const {
		return view;
	}

	void Image::Destroy()
	{
		Instance::GetInfo().device.destroyImageView(view);
		vmaDestroyImage(Instance::GetInfo().allocator, image, alloc);
	}
}