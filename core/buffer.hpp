#pragma once

#include <limits>

namespace REngine::Core {
	class Buffer {
		VmaAllocation alloc;
		vk::DeviceSize size;
		vk::Buffer buffer;
		
		public:
		void Create(vk::DeviceSize size, vk::BufferUsageFlags usage, bool mappable = false);
		void CopyData(const void *data, vk::DeviceSize size = std::numeric_limits<uint64_t>::max());
		void Copy(const vk::Image image, uint32_t width, uint32_t height);
		void Copy(Buffer dst) const;
		void Stage(void *data);
		const vk::Buffer &GetBuffer() const;
		vk::DeviceSize Size() const;
		void Destroy();
	};
}