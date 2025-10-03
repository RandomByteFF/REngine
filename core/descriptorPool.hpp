#pragma once

#include "headers.h"
#include "buffer.hpp"
#include "image.hpp"

namespace REngine::Core {
	class DescriptorPool {
		inline static std::vector<vk::DescriptorPool> pools;
		static void AllocatePool();
	public:
		static std::vector<vk::DescriptorSet> CreateDescriptor(vk::DescriptorSetLayout layout, uint32_t count);
		static void SetUniform(vk::DescriptorSet descriptorSet, uint32_t binding, Buffer uniformBuffer);
		static void SetImage(vk::DescriptorSet descriptorSet, uint32_t binding, Image image, vk::Sampler sampler);
		static void SetImage(vk::DescriptorSet descriptorSet, uint32_t binding, vk::ImageView image, vk::Sampler sampler);
		static void Cleanup();
	};
}