#include "commandBuffer.hpp"

#include "instance.hpp"

namespace REngine::Core {
	void CommandBuffer::Create(vk::RenderPass renderPass) {
		this->renderPass = renderPass;
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.commandPool = Instance::GetInfo().commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		commandBuffer = Instance::GetInfo().device.allocateCommandBuffers(allocInfo)[0];
	}

	void CommandBuffer::Reset() {
		commandBuffer.reset();
	}

	void CommandBuffer::BeginPass(vk::Extent2D extent, vk::Framebuffer frameBuffer) {
		vk::CommandBufferBeginInfo info{};
		commandBuffer.begin(info);
		vk::RenderPassBeginInfo renderPassInfo{};
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = frameBuffer;
		renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
		renderPassInfo.renderArea.extent = extent;

		std::array<vk::ClearValue, 2> clearValues{};
		clearValues[0].setColor(vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f));
		clearValues[1].depthStencil = {{1.0f, 0}};
		renderPassInfo.clearValueCount = uint32_t(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();
		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		vk::Viewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = float(extent.width);
		viewport.height = float(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		commandBuffer.setViewport(0, viewport);
		
		vk::Rect2D scissor{};
		scissor.offset = {{0, 0}};
		scissor.extent = extent;
		commandBuffer.setScissor(0, scissor);
		
	}
	
	void CommandBuffer::End() {
		commandBuffer.endRenderPass();
		commandBuffer.end();
	}
}