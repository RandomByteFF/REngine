#include "object.hpp"
#include "descriptorPool.hpp"
#include "instance.hpp"

namespace REngine::Core {
	void Object::Create(Pipeline pipeline) {
		this->pipeline = pipeline;
		descriptorSets = DescriptorPool::CreateDescriptor(pipeline.GetLayout(), Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
		uniformBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			uniformBuffers[i].Create(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, true);
			DescriptorPool::SetUniform(descriptorSets[i], 0, uniformBuffers[i]);
		}
	}
	void Object::Bind(vk::CommandBuffer cb, uint32_t currentFrame) {
		cb.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.layout, 0, descriptorSets[currentFrame], nullptr);
	}
	void Object::SetImage(Image image, vk::Sampler sampler) {
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			DescriptorPool::SetImage(descriptorSets[i], 1, image, sampler);
		}
	}
	void Object::Destroy() {
		for (auto i : uniformBuffers) {
			i.Destroy();
		}
	}
}
