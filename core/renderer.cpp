#include "renderer.hpp"

#include "instance.hpp"
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include <iostream>

namespace REngine::Core {
	
	void Renderer::Create(WindowManager window) {
		this->window = window;
		device = Instance::GetInfo().device;

		swapchain.CreateSwapchain();
		colorFormat = swapchain.ImageFormat();
		depthFormat = FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
		
		CreateRenderPass();
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = Instance::Get();
		init_info.PhysicalDevice = Instance::GetInfo().physicalDevice;
		init_info.Device = Instance::GetInfo().device;
		init_info.QueueFamily = Instance::GetInfo().queues.graphicsFamily.value();
		init_info.Queue = Instance::GetInfo().graphicsQueue;
		init_info.DescriptorPoolSize = 100;
		init_info.MSAASamples = VkSampleCountFlagBits(Instance::GetInfo().maxMsaa);
		//TODO: understand why i need this here
		init_info.MinImageCount = uint32_t(GetSwapchain().Views().size());
		init_info.ImageCount = init_info.MinImageCount;
		init_info.RenderPass = RenderPass();
		ImGui_ImplVulkan_Init(&init_info);

		CreateImages();
		CreateSampler();
		
		viewportImages.resize(swapchain.SwapchainImageCount());
		for (size_t i = 0; i < swapchain.SwapchainImageCount(); i++) {
			viewportImages[i].CreateImage(swapchain.Extent().width, swapchain.Extent().height, 1, vk::SampleCountFlagBits::e1, 
			swapchain.ImageFormat(), vk::ImageTiling::eLinear, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
			viewportImages[i].TransitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
		}
		renderedViewports.resize(viewportImages.size());
		for (size_t i = 0; i < viewportImages.size(); i++) {	
			renderedViewports[i] = ImGui_ImplVulkan_AddTexture(sampler, viewportImages[i].View(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		CreateFrameBuffers();
		CreateSyncObjects();
			
		commandBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		// viewportCommandBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			commandBuffers[i].Create();
			// commandBuffers[i].Create(viewportRenderPass);
		}
		
	}

	void Renderer::CreateImages() {
		colorImage.CreateImage(swapchain.Extent().width, swapchain.Extent().height, 1, Instance::GetInfo().maxMsaa, vk::Format(colorFormat), 
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
		depthImage.CreateImage(swapchain.Extent().width, swapchain.Extent().height, 1, Instance::GetInfo().maxMsaa, vk::Format(depthFormat), 
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);
	}

	void Renderer::CreateFrameBuffers() {
		swapChainFrameBuffers.resize(swapchain.Views().size());
		viewportFrambuffers.resize(swapchain.Views().size());
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

			std::array<vk::ImageView, 3> attachments2 = {
				colorImage.View(),
				depthImage.View(),
				viewportImages[i].View()
			};
			frameBufferInfo.pAttachments = attachments2.data();
			viewportFrambuffers[i] = device.createFramebuffer(frameBufferInfo);
		}
	}

	const Swapchain Renderer::GetSwapchain() const {
		return swapchain;
	}

	const vk::RenderPass Renderer::RenderPass() const {
		return renderPass;
	}

	const vk::Sampler Renderer::Sampler() const {
		return sampler;
	}

	void Renderer::Render(std::vector<Mesh> &objects, Camera &camera) {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Begin("Editor");
		if (i++ > 0) {
			ImGui::Image(ImTextureID(VkDescriptorSet(renderedViewports[currentFrame])), ImVec2(300, 300));
		}
		ImGui::End();
		vk::Result res = device.waitForFences(1, &inFlightFences[currentFrame], true, std::numeric_limits<uint64_t>::max());
		if (res != vk::Result::eSuccess) throw std::runtime_error("Failed to wait for fence");
		
		uint32_t imageIndex;
		try {
			vk::ResultValue<uint32_t> result = device.acquireNextImageKHR(swapchain.GetSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], {});
			imageIndex = result.value;
			
			if (result.result == vk::Result::eSuboptimalKHR) {
				RecreateSwapchain();
				return;
			}
			else if (result.result != vk::Result::eSuccess) {
				throw std::runtime_error("Failed to acquire swap chain image!");
			}
		}
		catch (vk::OutOfDateKHRError error) {
			RecreateSwapchain();
			return;
		}

		res = device.resetFences(1, &inFlightFences[currentFrame]);
		if (res != vk::Result::eSuccess) throw std::runtime_error("Failed to reset fence");

		commandBuffers[currentFrame].Reset();
		commandBuffers[currentFrame].Begin();
		commandBuffers[currentFrame].BeginPass(viewportRenderPass, swapchain.Extent(), viewportFrambuffers[imageIndex]);

		for (auto i : objects) {
			i.Update(camera);
			i.Bind(commandBuffers[currentFrame].GetBuffer());
			i.Draw(commandBuffers[currentFrame].GetBuffer());
		}


		
		commandBuffers[currentFrame].EndPass();
		
		// Submitting graphics draw
		vk::MemoryBarrier memoryBarrier{};
		memoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		memoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentRead;
		
		vk::PipelineStageFlags sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests;
		vk::PipelineStageFlags destinationStage = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eVertexShader;
		
		commandBuffers[currentFrame].BeginPass(renderPass, swapchain.Extent(), swapChainFrameBuffers[imageIndex]);
		
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[currentFrame].GetBuffer());
		commandBuffers[currentFrame].EndPass();
		commandBuffers[currentFrame].End();
		
		// Submitting UI draw
		// submitInfo.pCommandBuffers = &commandBuffers[currentFrame].GetBuffer();
		// vk::Semaphore waitSemaphores2[] = {renderFinishedSemaphores[currentFrame]};
		// submitInfo.pWaitSemaphores = waitSemaphores2;
		// vk::Semaphore signalSemaphores2[] = {uiRenderFinishedSemaphores[currentFrame]};
		// submitInfo.pSignalSemaphores = signalSemaphores2;
		
		vk::SubmitInfo submitInfo{};

		vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame].GetBuffer();

		vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		//Instance::GetInfo().graphicsQueue.submit(submitInfo);
		Instance::GetInfo().graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
		
		vk::PresentInfoKHR presentInfo{};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		vk::SwapchainKHR swapChains[] = {swapchain.GetSwapchain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		try {
			vk::Result res2 = Instance::GetInfo().presentQueue.presentKHR(presentInfo);
			if (res2 == vk::Result::eSuboptimalKHR || window.IsDirty(true)) {
				RecreateSwapchain();
			}
			else if (res2 != vk::Result::eSuccess) {
				throw std::runtime_error("Failed to acquire swap chain image!");
			}
		}
		catch (vk::OutOfDateKHRError error) {
			RecreateSwapchain();
		}
		
		
		currentFrame = (currentFrame + 1) % Instance::GetInfo().MAX_FRAMES_IN_FLIGHT;
		Instance::SetCurrentFrame(currentFrame);
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
		colorAttachmentResolve.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		
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
		
		viewportRenderPass = Instance::GetInfo().device.createRenderPass(renderPassInfo);
		attachments[2].finalLayout = vk::ImageLayout::ePresentSrcKHR;
		renderPassInfo.pAttachments = attachments.data();
		renderPass = Instance::GetInfo().device.createRenderPass(renderPassInfo);
	}

	void Renderer::CreateSampler() {
		vk::SamplerCreateInfo samplerInfo{};
		samplerInfo.magFilter = vk::Filter::eLinear;
		samplerInfo.minFilter = vk::Filter::eLinear;
		samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
		samplerInfo.anisotropyEnable = VK_TRUE;

		auto properties = Instance::GetInfo().physicalDevice.getProperties();

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = vk::CompareOp::eAlways;
		samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0;
		samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

		sampler = device.createSampler(samplerInfo);
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

		device.destroySampler(sampler);
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
		CreateImages();
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