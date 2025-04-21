#include "renderer.hpp"

#include "instance.hpp"
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include <iostream>

namespace REngine::Core {
	
	void Renderer::Create(WindowManager window, std::vector<std::shared_ptr<Drawable>> &objects) {
		this->window = window;
		device = Instance::GetInfo().device;

		swapchain.CreateSwapchain();
		colorFormat = swapchain.ImageFormat();
		depthFormat = FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
		
		CreateSampler();
		CreateImages();
		
		vpRenderer.CreateRenderPass();
		
		vpRenderer.CreateFramebuffers();
		CreateSyncObjects();
		
		commandBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			commandBuffers[i].Create();
		}
		
		#ifdef EDITOR
		editor.Initialize(swapchain, vpRenderer.RenderPass(), objects);
		editor.CreateFramebuffers(swapchain);
		editor.AddTextures(viewportView, sampler);
		barrier.oldLayout = vk::ImageLayout::ePresentSrcKHR;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;
		#endif
	}

	void Renderer::CreateImages() {
		#ifdef EDITOR
		viewportImages.resize(swapchain.SwapchainImageCount());
		for (size_t i = 0; i < swapchain.SwapchainImageCount(); i++) {
			// TODO: don't set it to swapchain's extent, it should be the image extent
			viewportImages[i].CreateImage(swapchain.Extent().width, swapchain.Extent().height, 1, vk::SampleCountFlagBits::e1, 
			colorFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);
			viewportImages[i].TransitionLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
		}
		for(auto i : viewportImages) viewportView.push_back(i.View());
		vpRenderer.CreateImages(swapchain.Extent(), swapchain.ImageFormat(), depthFormat, viewportView);
		#else
		vpRenderer.CreateImages(swapchain.Extent(), swapchain.ImageFormat(), depthFormat, swapchain.Views());
		#endif
	}

	const Swapchain Renderer::GetSwapchain() const {
		return swapchain;
	}

	const vk::RenderPass Renderer::RenderPass() const {
		return vpRenderer.RenderPass();
	}

	const vk::Sampler Renderer::Sampler() const {
		return sampler;
	}

	void Renderer::Render(std::vector<std::shared_ptr<Drawable>> &objects, Camera &camera) {
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
			#ifdef EDITOR
				ImGui::Render();
			#endif
			RecreateSwapchain();
			return;
		}

		res = device.resetFences(1, &inFlightFences[currentFrame]);
		if (res != vk::Result::eSuccess) throw std::runtime_error("Failed to reset fence");

		commandBuffers[currentFrame].Reset();
		commandBuffers[currentFrame].Begin();
	
		vpRenderer.Render(commandBuffers[currentFrame], swapchain.Extent(), imageIndex, objects, camera);

		#ifdef EDITOR
		barrier.image = viewportImages[imageIndex].Get();
		commandBuffers[currentFrame].GetBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), nullptr, nullptr, barrier);

		editor.Render(imageIndex, commandBuffers[currentFrame], swapchain.Extent());
		#endif
		
		commandBuffers[currentFrame].End();
		
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
		#ifdef EDITOR
		editor.Destroy();
		#endif
		vpRenderer.Destroy();
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
		vpRenderer.CreateFramebuffers();
		#ifdef EDITOR
		editor.AddTextures(viewportView, sampler);
		editor.CreateFramebuffers(swapchain);
		#endif
	}

	float Renderer::AspectRatio() {
		return float(swapchain.Extent().width) / float(swapchain.Extent().height);
	}

	void Renderer::CleanupSwapchain() {
		vpRenderer.DestroyBuffers();
		#ifdef EDITOR
		editor.DestroyBuffers();
		for (auto image : viewportImages) {
			image.Destroy();
		}
		viewportView.clear();
		#endif
		swapchain.Destroy();
	}
}