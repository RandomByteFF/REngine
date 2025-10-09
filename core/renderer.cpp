#include "renderer.hpp"

#include "instance.hpp"
#include "imgui.h"
#include "scene/drawable.hpp"
#include "scene/sceneTree.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <memory>

namespace REngine::Core {
	
	void Renderer::Create() {
		device = Instance::GetInfo().device;

		swapchain = std::make_shared<Swapchain>();
		swapchain->CreateSwapchain();
		colorFormat = swapchain->ImageFormat();

		vpRenderer.AddColorAttachment().samples = Instance::GetInfo().maxMsaa;
		vpRenderer.AddColorImage();
		vpRenderer.AddDepthAttachment().samples = Instance::GetInfo().maxMsaa;
		vpRenderer.AddDepthImage();
		vk::AttachmentDescription &resolve = vpRenderer.AddResolveAttachment();
		#ifndef EDITOR
		resolve.finalLayout = vk::ImageLayout::ePresentSrcKHR;
		vpRenderer.AddResolveImage(swapchain);
		#else
		vpRenderer.AddResolveImage();
		#endif

		vpRenderer.CreateRenderPass();
		
		CreateSampler();
		CreateSyncObjects();
		
		commandBuffers.resize(Instance::GetInfo().MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < Instance::GetInfo().MAX_FRAMES_IN_FLIGHT; i++) {
			commandBuffers[i].Create();
		}
		
		#ifdef EDITOR
		editor.Initialize(swapchain, vpRenderer);
		editor.AddTextures(sampler);
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

	const Swapchain Renderer::GetSwapchain() const {
		return *swapchain;
	}

	const vk::RenderPass Renderer::GetRenderPass() const {
		return vpRenderer.GetRenderPass();
	}

	const vk::Sampler Renderer::Sampler() const {
		return sampler;
	}

	void Renderer::Render(Scene::SceneTree &sceneTree, Camera &camera) {
		vk::Result res = device.waitForFences(1, &inFlightFences[currentFrame], true, std::numeric_limits<uint64_t>::max());
		if (res != vk::Result::eSuccess) throw std::runtime_error("Failed to wait for fence");
		
		uint32_t imageIndex;
		try {
			vk::ResultValue<uint32_t> result = device.acquireNextImageKHR(swapchain->GetSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], {});
			imageIndex = result.value;
			Instance::SetCurrentFb(imageIndex);
			
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

		sceneTree.PreDraw(commandBuffers[currentFrame]);
	
		// vpRenderer.Render(commandBuffers[currentFrame], swapchain.Extent(), imageIndex, sceneTree, camera);
		commandBuffers[currentFrame].BeginPass(vpRenderer.GetRenderPass(), swapchain->Extent(), vpRenderer.GetFramebuffer()[imageIndex]);

		sceneTree.Draw(commandBuffers[currentFrame].GetBuffer());
		
		commandBuffers[currentFrame].EndPass();
		
		sceneTree.PostDraw(commandBuffers[currentFrame]);

		#ifdef EDITOR
		barrier.image = vpRenderer.GetImage(2, imageIndex);
		commandBuffers[currentFrame].GetBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), nullptr, nullptr, barrier);

		editor.Render(imageIndex, commandBuffers[currentFrame], swapchain->Extent());
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

		vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		Instance::GetInfo().graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);
		
		vk::PresentInfoKHR presentInfo{};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		vk::SwapchainKHR swapChains[] = {swapchain->GetSwapchain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		try {
			vk::Result res2 = Instance::GetInfo().presentQueue.presentKHR(presentInfo);
			if (res2 == vk::Result::eSuboptimalKHR || WindowManager::Instance()->IsDirty(true)) {
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
		uint32_t scimages = Instance::GetInfo().swapchainSize;
		vk::Device device = Instance::GetInfo().device;
		imageAvailableSemaphores.resize(frames);
		renderFinishedSemaphores.resize(scimages);
		inFlightFences.resize(frames);

		vk::SemaphoreCreateInfo semaphoreInfo{};

		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		for (size_t i = 0; i < frames; i++) {
			inFlightFences[i] = device.createFence(fenceInfo);
			imageAvailableSemaphores[i] = device.createSemaphore(semaphoreInfo);
		}
		for(size_t i = 0; i < scimages; i++) {
			renderFinishedSemaphores[i] = device.createSemaphore(semaphoreInfo);
		}
	}


	void Renderer::Destroy() {
		swapchain->Destroy();
		device.destroySampler(sampler);
		for (size_t i = 0; i < renderFinishedSemaphores.size(); i++)
		{
			device.destroySemaphore(renderFinishedSemaphores[i]);
		}
		for(size_t i = 0; i < inFlightFences.size(); i++) {
			device.destroySemaphore(imageAvailableSemaphores[i]);
			device.destroyFence(inFlightFences[i]);
		}
		vpRenderer.Destroy();
		#ifdef EDITOR
		editor.Destroy();
		#endif
	}

	void Renderer::RecreateSwapchain() {
		int width = 0, height = 0;
		WindowManager::Instance()->GetFrameBufferSize(width, height);
		while (width == 0 || height == 0) {
			WindowManager::Instance()->GetFrameBufferSize(width, height);
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(device);
		swapchain->Destroy();
		swapchain->CreateSwapchain();
		RenderTarget::RecreateAll();
		vpRenderer.Recreate();
		Scene::SceneTree::Current()->CallDrawlist([](Scene::Drawable &d) {
			d.Recreate();
		});
		#ifdef EDITOR
		editor.Recreate();
		#endif
	}

	float Renderer::AspectRatio() {
		return float(swapchain->Extent().width) / float(swapchain->Extent().height);
	}
}