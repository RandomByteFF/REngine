#include "image.hpp"

#include "instance.hpp"
#include "commandBuffer.hpp"
#include "buffer.hpp"

#include <cmath>

namespace REngine::Core {
	void Image::CreateImage(uint32_t width, uint32_t height, int mipLevels,
			vk::SampleCountFlagBits numSample, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage,
			vk::ImageAspectFlagBits aspect) {
		this->width = width;
		this->height = height;
		this->format = format;
		this->mipLevels = mipLevels;

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
		if (vmaCreateImage(Instance::GetInfo().allocator, &ii, &allocCreateInfo, &img, &alloc, &allocInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		image = img;
		view = CreateImageView(image, format, mipLevels, aspect);
	}

	void Image::CreateImage(int width, int height, int mipLevels, vk::DeviceSize size, unsigned char *pixels) {
		this->width = width;
		this->height = height;
		this->mipLevels = mipLevels;

		Buffer stagingBuffer;
		stagingBuffer.Create(size, vk::BufferUsageFlagBits::eTransferSrc, true);
		stagingBuffer.CopyData(pixels);
		auto format = Instance::GetInfo().imageFormat;
		//FIXME: please don't leave it like this
		vk::Format actualFormat = vk::Format::eR8G8B8A8Srgb;
		if (format.format != vk::Format::eB8G8R8A8Srgb) {
			actualFormat = vk::Format::eR8G8B8A8Unorm;
		}

		CreateImage(width, height, mipLevels, vk::SampleCountFlagBits::e1, actualFormat, vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst);

		TransitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
		stagingBuffer.Copy(image, width, height);
		//Doing this while generating mipmaps.
		//TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);
		GenerateMipmaps(mipLevels);

		stagingBuffer.Destroy();
	}

	void Image::CreateImage(const Loader::Image &image, int mipLevels) {
		if (mipLevels == 0) {
			mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(image.Width(), image.Height())))) + 1;
		}
		CreateImage(image.Width(), image.Height(), mipLevels, image.Size(), image.Pixels());
	}

	void Image::TransitionLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
		vk::CommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommands();
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
		else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;

			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else {
			throw std::invalid_argument("Unsupported layout transition!");
		}

		commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), nullptr, nullptr, barrier);

		CommandBuffer::EndSingleTimeCommands(commandBuffer);
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
	
	void Image::GenerateMipmaps(uint32_t mipLevels) {
		vk::FormatProperties formatProperties = Instance::GetInfo().physicalDevice.getFormatProperties(format);

		vk::CommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommands();

		vk::ImageMemoryBarrier barrier{};
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = width;
		int32_t mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, 
			vk::DependencyFlags(),
			{},
			{},
			{barrier});
			
			vk::ImageBlit blit{};
			blit.srcOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.srcOffsets[1] = vk::Offset3D{mipWidth, mipHeight, 1};
			blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = vk::Offset3D{0, 0, 0};
			blit.dstOffsets[1] = vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
			blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;
			commandBuffer.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal,
			blit, vk::Filter::eLinear);
			
			barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			
			commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, 
			vk::DependencyFlags(),
			{},
			{},
			{barrier});

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
			
		}
		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		
		commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, 
		vk::DependencyFlags(),
		{},
		{},
		{barrier});

		CommandBuffer::EndSingleTimeCommands(commandBuffer);
	}

	vk::ImageView Image::View() const {
		return view;
	}

	vk::Image Image::Get() const {
		return image;
	}

	vk::Format Image::Format() const {
		return format;
	}

	uint32_t Image::Width() const {
		return width;
	}

	uint32_t Image::Height() const {
		return height;
	}

	void Image::Destroy()
	{
		Instance::GetInfo().device.destroyImageView(view);
		vmaDestroyImage(Instance::GetInfo().allocator, image, alloc);
	}
}