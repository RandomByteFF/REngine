#include "buffer.hpp"

#include "instance.hpp"
#include "commandBuffer.hpp"

namespace REngine::Core {
	void Buffer::Create(vk::DeviceSize size, vk::BufferUsageFlags usage, bool mappable) {
		vk::BufferCreateInfo bufferInfo{};
		this->size = size;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		if (mappable) allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

		VkBufferCreateInfo bi = bufferInfo;
		VkBuffer b;
		vmaCreateBuffer(Instance::GetInfo().allocator, &bi, &allocInfo, &b, &alloc, nullptr);
		buffer = b;
	}

	void Buffer::CopyData(const void *data, vk::DeviceSize size) {
		if (size == std::numeric_limits<uint64_t>::max()) size = this->size;
		vmaCopyMemoryToAllocation(Instance::GetInfo().allocator, data, alloc, 0, size);
	}

	void Buffer::Copy(const vk::Image image, uint32_t width, uint32_t height) {
		vk::CommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommands();

		vk::BufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0,
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = vk::Offset3D{0, 0, 0};
		region.imageExtent = vk::Extent3D{width, height, 1};

		commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

		CommandBuffer::EndSingleTimeCommands(commandBuffer);
	}

	void Buffer::Copy(Buffer dst) const {
		vk::CommandBuffer commandBuffer = CommandBuffer::BeginSingleTimeCommands();

		vk::BufferCopy copyRegion{};
		copyRegion.size = size;
		commandBuffer.copyBuffer(buffer, dst.buffer, copyRegion);		

		CommandBuffer::EndSingleTimeCommands(commandBuffer);
	}

	void Buffer::Stage(void *data) {
		Buffer staging;
		staging.Create(size, vk::BufferUsageFlagBits::eTransferSrc, true);
		staging.CopyData(data);
		staging.Copy(*this);
		staging.Destroy();
	}

	const vk::Buffer &Buffer::GetBuffer() const {
		return buffer;
	}

	vk::DeviceSize Buffer::Size() const {
		return size;
	}

	void Buffer::Destroy() {
		vmaDestroyBuffer(Instance::GetInfo().allocator, buffer, alloc);
	}
}