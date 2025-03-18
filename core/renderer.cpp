#include "renderer.hpp"

#include "instance.hpp"

namespace REngine::Core {
	
	void Renderer::Create(WindowManager window) {
		this->window = window;
		device = Instance::GetInfo().device;

		swapchain.CreateSwapchain();
		colorFormat = swapchain.ImageFormat();
		depthFormat = FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
			
		CreateSwapchain();
		CreateRenderPass();
		CreateFrameBuffers();
		CreateSyncObjects();
		commandBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			commandBuffers[i].Create(renderPass);
		}
	}
	
	void Renderer::CreateSwapchain() {
		colorImage.CreateImage(swapchain.Extent().width, swapchain.Extent().height, 1, Instance::GetInfo().maxMsaa, vk::Format(colorFormat), 
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
		depthImage.CreateImage(swapchain.Extent().width, swapchain.Extent().height, 1, Instance::GetInfo().maxMsaa, vk::Format(depthFormat), 
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
	}

	void Renderer::CreateFrameBuffers() {
		swapChainFrameBuffers.resize(swapchain.Views().size());
		for (size_t i = 0; i < swapchain.Views().size(); i++) {
			std::array<vk::ImageView, 3> attachments = {
				colorImage.View(),
				depthImage.View(),
				swapchain.Views()[i]
			};

			vk::FramebufferCreateInfo frameBufferInfo{};
			frameBufferInfo.renderPass = renderPass;
			frameBufferInfo.attachmentCount = uint32_t(attachments.size());
			frameBufferInfo.pAttachments = attachments.data();
			frameBufferInfo.width = swapchain.Extent().width;
			frameBufferInfo.height = swapchain.Extent().height;
			frameBufferInfo.layers = 1;

			swapChainFrameBuffers[i] = device.createFramebuffer(frameBufferInfo);
		}
	}

	const Swapchain Renderer::Swapchain() const {
		return swapchain;
	}

	const vk::RenderPass Renderer::RenderPass() const {
		return renderPass;
	}

	void Renderer::Render(std::vector<Object> objects, Buffer vertexBuffer, Buffer indexBuffer, uint32_t indicesSize) {
		device.waitForFences(1, &inFlightFences[currentFrame], true, std::numeric_limits<uint64_t>::max());
		
		uint32_t imageIndex;
		vk::ResultValue<uint32_t> result = device.acquireNextImageKHR(swapchain.GetSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], {});
		imageIndex = result.value;
		
		if (result.result == vk::Result::eErrorOutOfDateKHR || result.result == vk::Result::eSuboptimalKHR) {
			RecreateSwapchain();
			return;
		}
		else if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		device.resetFences(1, &inFlightFences[currentFrame]);

		commandBuffers[currentFrame].Reset();
		commandBuffers[currentFrame].BeginPass(swapchain.Extent(), swapChainFrameBuffers[imageIndex]);

		//TODO: Bind objects
		std::array<vk::Buffer, 1> vertexBuffers = {vertexBuffer.buffer};
		vk::DeviceSize offset[] = {0};
		commandBuffers[currentFrame].commandBuffer.bindVertexBuffers(0, vertexBuffers, offset);
		commandBuffers[currentFrame].commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, vk::IndexType::eUint32);
		commandBuffers[currentFrame].commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, objects[0].pipeline.pipeline);
		objects[0].Bind(commandBuffers[currentFrame].commandBuffer, currentFrame);

		commandBuffers[currentFrame].commandBuffer.drawIndexed(indicesSize, 1, 0, 0, 0);

		commandBuffers[currentFrame].End();
		// UpdateUniformBuffer(currentFrame);

		vk::SubmitInfo submitInfo{};

		vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame].commandBuffer;

		vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		Instance::GetInfo().graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

		vk::PresentInfoKHR presentInfo{};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		vk::SwapchainKHR swapChains[] = {swapchain.GetSwapchain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vk::Result res2 = Instance::GetInfo().presentQueue.presentKHR(presentInfo);

		currentFrame = (currentFrame + 1) % Instance::GetInfo().MAX_FRAMES_IN_FLIGHT;

		if (res2 == vk::Result::eErrorOutOfDateKHR || res2 == vk::Result::eSuboptimalKHR || window.IsDirty(true)) {
			RecreateSwapchain();
		}
		else if (res2 != vk::Result::eSuccess) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}
	}

	void Renderer::CreateRenderPass() {
		vk::AttachmentDescription colorAttachment{};
		colorAttachment.format = swapchain.ImageFormat();
		colorAttachment.samples = Instance::GetInfo().maxMsaa;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentDescription depthAttachment{};
		depthAttachment.format = depthFormat;
		depthAttachment.samples = Instance::GetInfo().maxMsaa;
		depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
		depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = swapchain.ImageFormat();
		colorAttachmentResolve.samples = vk::SampleCountFlagBits::e1;
		colorAttachmentResolve.loadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachmentResolve.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachmentResolve.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachmentResolve.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachmentResolve.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachmentResolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

		vk::AttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass{};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;

		std::array<vk::AttachmentDescription, 3> attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};

		vk::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.attachmentCount = uint32_t(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		vk::SubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.srcAccessMask = vk::AccessFlagBits::eNone;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		renderPass = Instance::GetInfo().device.createRenderPass(renderPassInfo);
	}

	void Renderer::CreateSyncObjects() {
		uint32_t frames = Instance::GetInfo().MAX_FRAMES_IN_FLIGHT;
		vk::Device device = Instance::GetInfo().device;
		imageAvailableSemaphores.resize(frames);
		renderFinishedSemaphores.resize(frames);
		inFlightFences.resize(frames);

		vk::SemaphoreCreateInfo semaphoreInfo{};

		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		for (size_t i = 0; i < frames; i++)
		{
			imageAvailableSemaphores[i] = device.createSemaphore(semaphoreInfo);
			renderFinishedSemaphores[i] = device.createSemaphore(semaphoreInfo);
			inFlightFences[i] = device.createFence(fenceInfo);
		}
	}

	vk::Format Renderer::FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
		for (vk::Format format :candidates) {
			auto props = Instance::GetInfo().physicalDevice.getFormatProperties(format);
			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("Failed to find supported format!");
	}

	void Renderer::Destroy() {
		CleanupSwapchain();

		for (size_t i = 0; i < renderFinishedSemaphores.size(); i++)
		{
			device.destroySemaphore(imageAvailableSemaphores[i]);
			device.destroySemaphore(renderFinishedSemaphores[i]);
			device.destroyFence(inFlightFences[i]);	
		}
		device.destroyRenderPass(renderPass);
	}

	void Renderer::RecreateSwapchain() {
		int width = 0, height = 0;
		window.GetFrameBufferSize(width, height);
		while (width == 0 || height == 0) {
			window.GetFrameBufferSize(width, height);
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(device);
		CleanupSwapchain();

		swapchain.CreateSwapchain();
		CreateSwapchain();
		CreateFrameBuffers();
	}

	void Renderer::CleanupSwapchain() {
		colorImage.Destroy();
		depthImage.Destroy();
		for (auto framebuffer : swapChainFrameBuffers) {
			device.destroyFramebuffer(framebuffer);
		}
		for (auto imageView : swapchain.Views()) {
			device.destroyImageView(imageView);
		}
		device.destroySwapchainKHR(swapchain.GetSwapchain());
	}
}