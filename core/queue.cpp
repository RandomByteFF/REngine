#include "queue.hpp"

#include "instance.hpp"

namespace REngine::Core {
	vk::CommandBuffer Queue::BeginSingleTimeCommands() {
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = Instance::GetInfo().commandPool;
		allocInfo.commandBufferCount = 1;

		vk::CommandBuffer commandBuffer = Instance::GetInfo().device.allocateCommandBuffers(allocInfo)[0];

		vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		commandBuffer.begin(beginInfo);
		
		return commandBuffer;
	}
	
	void Queue::EndSingleTimeCommands(vk::CommandBuffer commandBuffer) {
		commandBuffer.end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		auto gq = Instance::GetInfo().graphicsQueue;
		gq.submit(submitInfo);
		gq.waitIdle();
		// FIXME: do i really need this?
		Instance::GetInfo().device.freeCommandBuffers(Instance::GetInfo().commandPool, commandBuffer);
	}
}