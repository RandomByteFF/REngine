#include "editor.hpp"

#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "core/instance.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "scene/sceneTree.hpp"

namespace REngine::Editor {
	void Editor::Initialize(Core::Swapchain swapchain, vk::RenderPass vpRenderPass) {
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
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		grid = std::shared_ptr<Grid>(new Grid());
		grid->Create(swapchain, vpRenderPass);
		grid->SetDrawOrder(1);
		Scene::SceneTree::Current()->GetRoot()->AddChild(grid);
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
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGui::Begin("REngine", nullptr, window_flags);
		ImGui::End();
		ImGui::PopStyleVar(2);
		ImGui::Begin("Editor");
		ImVec2 size = ImGui::GetWindowSize();
		size.x -= 20;
		size.y -= 40;
		if (size.x != prevViewSize.x || size.y != prevViewSize.y) {
			Core::Instance::FrameBufferResized(int(size.x), int(size.y));
			prevViewSize = size;
		}
		ImGui::Image(ImTextureID(VkDescriptorSet(renderedViewports[imageIndex])), size);
		ImGui::End();
			
		ImGui::Begin("Debug");
		ImGui::Text(Input::Mouse::IsDown(GLFW_MOUSE_BUTTON_LEFT) ? "Pressed" : "Not pressed");
		if (Input::Keyboard::IsDown(GLFW_KEY_1)) Input::Mouse::Lock();
		if (Input::Keyboard::IsDown(GLFW_KEY_ESCAPE)) Input::Mouse::Unlock();
		glm::vec2 cursor = Input::Mouse::CursorPos();
		ImGui::Text(std::format("Cursor position: X: {}, Y: {}", cursor.x, cursor.y).c_str());
		glm::vec2 delta = Input::Mouse::Delta();
		ImGui::Text(std::format("Cursor delta: X: {}, Y: {}", delta.x, delta.y).c_str());
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