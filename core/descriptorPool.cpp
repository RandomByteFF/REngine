#include "descriptorPool.hpp"

#include "instance.hpp"
#include <iostream>

namespace REngine::Core {
	void DescriptorPool::AllocatePool() {
		// TODO: something a bit less wasteful
		std::array<vk::DescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = 1000;
		poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
		poolSizes[1].descriptorCount = 1000;

		vk::DescriptorPoolCreateInfo poolInfo{};
		poolInfo.poolSizeCount = uint32_t(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = uint32_t(1000);

		pools.push_back(Instance::GetInfo().device.createDescriptorPool(poolInfo));
	}

	std::vector<vk::DescriptorSet> DescriptorPool::CreateDescriptor(vk::DescriptorSetLayout layout, uint32_t count) {
		if (pools.size() == 0) AllocatePool();
		std::vector<vk::DescriptorSetLayout> layouts(count, layout);
		vk::DescriptorSetAllocateInfo allocInfo{};
		allocInfo.descriptorPool = pools[pools.size() - 1];
		allocInfo.descriptorSetCount = uint32_t(count);
		allocInfo.pSetLayouts = layouts.data();
		std::vector<vk::DescriptorSet> descriptorSets;
		try {
			descriptorSets = Instance::GetInfo().device.allocateDescriptorSets(allocInfo);
		}
		catch (vk::OutOfDeviceMemoryError e){
			AllocatePool();
			std::cout << "Allocating new pool" << std::endl;
			descriptorSets = Instance::GetInfo().device.allocateDescriptorSets(allocInfo);
		}
		return descriptorSets;
	}

	void DescriptorPool::SetUniform(vk::DescriptorSet descriptorSet, uint32_t binding, Buffer uniformBuffer) {
		vk::DescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffer.GetBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBuffer.Size();

		vk::WriteDescriptorSet descriptorWrites;
		descriptorWrites.dstSet = descriptorSet;
		descriptorWrites.dstBinding = binding;
		descriptorWrites.dstArrayElement = 0;
		descriptorWrites.descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrites.descriptorCount = 1;
		descriptorWrites.pBufferInfo = &bufferInfo;

		Instance::GetInfo().device.updateDescriptorSets(descriptorWrites, nullptr);
	}
	void DescriptorPool::SetImage(vk::DescriptorSet descriptorSet, uint32_t binding, Image image, vk::Sampler sampler) {
		vk::DescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView = image.View();
		imageInfo.sampler = sampler;
		
		vk::WriteDescriptorSet descriptorWrites;
		descriptorWrites.dstSet = descriptorSet;
		descriptorWrites.dstBinding = binding;
		descriptorWrites.dstArrayElement = 0;
		descriptorWrites.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites.descriptorCount = 1;
		descriptorWrites.pImageInfo = &imageInfo;

		Instance::GetInfo().device.updateDescriptorSets(descriptorWrites, nullptr);
	}

	void DescriptorPool::Cleanup() {
		for (auto i : pools) {
			Instance::GetInfo().device.destroyDescriptorPool(i);
		}
		pools.clear();
	}
}