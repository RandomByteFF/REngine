#include "editor.hpp"

#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "core/instance.hpp"

namespace REngine::Editor {
	void Editor::Initialize(Core::Swapchain swapchain) {
		auto info = Core::Instance::GetInfo();
		vk::AttachmentDescription colorAttachment{};
		colorAttachment.format = swapchain.ImageFormat();
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
		
		vk::AttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
		
		vk::SubpassDescription subpass{};
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		
		vk::RenderPassCreateInfo renderPassInfo{};
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		
		renderPass = info.device.createRenderPass(renderPassInfo);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = Core::Instance::Get();
		init_info.PhysicalDevice = info.physicalDevice;
		init_info.Device = info.device;
		init_info.QueueFamily = info.queues.graphicsFamily.value();
		init_info.Queue = info.graphicsQueue;
		init_info.DescriptorPoolSize = 100;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		//TODO: understand why i need this here
		init_info.MinImageCount = uint32_t(swapchain.Views().size());
		init_info.ImageCount = init_info.MinImageCount;
		init_info.RenderPass = renderPass;
		ImGui_ImplVulkan_Init(&init_info);

	}
	
	void Editor::CreateFramebuffers(Core::Swapchain swapchain) {
		framebuffers.resize(swapchain.Views().size());
		for (size_t i = 0; i < swapchain.Views().size(); i++) {
			vk::FramebufferCreateInfo frameBufferInfo{};
			frameBufferInfo.renderPass = renderPass;
			frameBufferInfo.attachmentCount = 1;
			frameBufferInfo.pAttachments = &(swapchain.Views()[i]);
			frameBufferInfo.width = swapchain.Extent().width;
			frameBufferInfo.height = swapchain.Extent().height;
			frameBufferInfo.layers = 1;
	
			framebuffers[i] = Core::Instance::GetInfo().device.createFramebuffer(frameBufferInfo);
		}
	}

	void Editor::AddTextures(std::vector<vk::ImageView> &views, vk::Sampler sampler) {
		renderedViewports.resize(views.size());
		for (size_t i = 0; i < views.size(); i++) {	
			renderedViewports[i] = ImGui_ImplVulkan_AddTexture(sampler, views[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	void Editor::Render(uint32_t imageIndex, Core::CommandBuffer cb, vk::Extent2D extent) {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Begin("Editor");
		ImGui::Image(ImTextureID(VkDescriptorSet(renderedViewports[imageIndex])), ImVec2(300, 300));
		ImGui::End();

		ImGui::Render();
		
		cb.BeginPass(renderPass, extent, framebuffers[imageIndex]);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb.GetBuffer());
		cb.EndPass();
	}

	void Editor::DestroyBuffers() {
		for (auto i : framebuffers) Core::Instance::GetInfo().device.destroyFramebuffer(i);
		for (auto i : renderedViewports) ImGui_ImplVulkan_RemoveTexture(i);
	}

	void Editor::Destroy() {
		Core::Instance::GetInfo().device.destroyRenderPass(renderPass);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}