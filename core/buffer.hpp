#pragma once

#include "headers.h"

namespace REngine::Core {
	class Buffer {
		//TODO: buffer should know it's own size
		VmaAllocation alloc;
		public:
		vk::Buffer buffer;
		void Create(vk::DeviceSize size, vk::BufferUsageFlags usage, bool mappable = false);
		void Copy(const void *data, vk::DeviceSize size);
		void Copy(vk::Image image, uint32_t width, uint32_t height);
		void Copy(Buffer dst, vk::DeviceSize size);
		void Destroy();
	};
}