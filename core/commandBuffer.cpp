#include "commandBuffer.hpp"

#include "instance.hpp"

namespace REngine::Core {
	void CommandBuffer::Create() {
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.commandPool = Instance::GetInfo().commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		commandBuffer = Instance::GetInfo().device.allocateCommandBuffers(allocInfo)[0];
	}

	void CommandBuffer::Reset() {
		commandBuffer.reset();
	}

	void CommandBuffer::Begin() {
		vk::CommandBufferBeginInfo info{};
		commandBuffer.begin(info);
	}

	void CommandBuffer::End() {
		commandBuffer.end();
	}

	vk::CommandBuffer &CommandBuffer::GetBuffer() {
		return commandBuffer;
	}

	vk::CommandBuffer CommandBuffer::BeginSingleTimeCommands() {
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = Instance::GetInfo().commandPool;
		allocInfo.commandBufferCount = 1;

		vk::CommandBuffer commandBuffer = Instance::GetInfo().device.allocateCommandBuffers(allocInfo)[0];

		vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffer.begin(beginInfo);
		
		return commandBuffer;
	}
	
	void CommandBuffer::EndSingleTimeCommands(vk::CommandBuffer commandBuffer) {
		commandBuffer.end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		auto gq = Instance::GetInfo().graphicsQueue;
		gq.submit(submitInfo);
		// TODO: switch this with a fence
		gq.waitIdle();
		Instance::GetInfo().device.freeCommandBuffers(Instance::GetInfo().commandPool, commandBuffer);

	}
}