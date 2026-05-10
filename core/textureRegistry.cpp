#include "textureRegistry.hpp"
#include "commandBuffer.hpp"
#include "instance.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"
#include <stdexcept>

namespace REngine::Core {
	void TextureRegistry::Create() {
		instance = this;
		auto device = Instance::GetInfo().device;

		vk::DescriptorPoolSize poolSize{};
		poolSize.type = vk::DescriptorType::eCombinedImageSampler;
		poolSize.descriptorCount = MAX_TEXTURES;

		vk::DescriptorPoolCreateInfo poolInfo{};
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
		poolInfo.maxSets = 1;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;

		pool = device.createDescriptorPool(poolInfo);

		vk::DescriptorSetLayoutBinding binding{};
		binding.binding = 0;
		binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		binding.descriptorCount = MAX_TEXTURES;
		binding.stageFlags = vk::ShaderStageFlagBits::eAll;

		vk::DescriptorBindingFlags bindingFlags = 
			vk::DescriptorBindingFlagBits::ePartiallyBound |
			vk::DescriptorBindingFlagBits::eVariableDescriptorCount |
			vk::DescriptorBindingFlagBits::eUpdateAfterBind;

		vk::DescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{};
		flagsInfo.bindingCount = 1;
		flagsInfo.pBindingFlags = &bindingFlags;

		vk::DescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.pNext = &flagsInfo;
		layoutInfo.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;

		layout = device.createDescriptorSetLayout(layoutInfo);

		uint32_t maxBinding = MAX_TEXTURES;

		vk::DescriptorSetVariableDescriptorCountAllocateInfo countInfo{};
		countInfo.descriptorSetCount = 1;
		countInfo.pDescriptorCounts = &maxBinding;

		vk::DescriptorSetAllocateInfo allocInfo{};
		allocInfo.pNext = &countInfo;
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		set = device.allocateDescriptorSets(allocInfo)[0];
		
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{}; // FIXME: yeah, this cant stay
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &layout;
		
		vk::PushConstantRange pushConstant;
		pushConstant.offset = 0;
		pushConstant.size = 128;
		pushConstant.stageFlags = vk::ShaderStageFlagBits::eAll;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
		
		pipelineLayout = Instance::GetInfo().device.createPipelineLayout(pipelineLayoutInfo);
	}

	void TextureRegistry::Bind(CommandBuffer cmd) {
		cmd.GetBuffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &set, 0, nullptr);
	}

	void TextureRegistry::Destroy() {
		auto device = Instance::GetInfo().device;
		device.destroyPipelineLayout(pipelineLayout);
		device.destroyDescriptorSetLayout(layout);
		device.destroyDescriptorPool(pool);
		nextSlot = 0;
		freeSlots.clear();
		instance = nullptr;
	}

	uint32_t TextureRegistry::Register(vk::ImageView view, vk::Sampler sampler) {
		uint32_t index;
		if (!freeSlots.empty()) {
			index = freeSlots.back();
			freeSlots.pop_back();
		} else {
			if (nextSlot >= MAX_TEXTURES) throw std::runtime_error("Bindless texture limit reached");
			index = nextSlot++;
		}

		vk::DescriptorImageInfo imageInfo{};
		imageInfo.sampler = sampler;
		imageInfo.imageView = view;
		imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		vk::WriteDescriptorSet write{};
		write.dstSet = set;
		write.dstBinding = 0;
		write.dstArrayElement = index;
		write.descriptorCount = 1;
		write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		write.pImageInfo = &imageInfo;

		Instance::GetInfo().device.updateDescriptorSets(write, {});

		return index;
	}

	void TextureRegistry::Free(uint32_t index) {
		freeSlots.push_back(index);
	}
	
	TextureRegistry &TextureRegistry::Instance() {
		return *instance;
	}
}