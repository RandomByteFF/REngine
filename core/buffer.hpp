#pragma once

#include "headers.h"
#include <limits>

namespace REngine::Core {
	class Buffer {
		VmaAllocation alloc;
		
		public:
		vk::DeviceSize size;
		vk::Buffer buffer;
		void Create(vk::DeviceSize size, vk::BufferUsageFlags usage, bool mappable = false);
		void CopyData(const void *data, vk::DeviceSize size = std::numeric_limits<uint64_t>::max());
		void Copy(vk::Image image, uint32_t width, uint32_t height);
		void Copy(Buffer dst);
		void Stage(void *data);
		void Destroy();
	};
}