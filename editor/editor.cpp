#include "editor.hpp"

#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "core/instance.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "scene/sceneTree.hpp"
#include "theme.hpp"
#include <iostream>

namespace REngine::Editor {
	void Editor::Initialize(std::shared_ptr<Core::Swapchain> swapchain, Core::RenderPass vpRenderPass) {
		renderPass.AddColorAttachment().finalLayout = vk::ImageLayout::ePresentSrcKHR;
		renderPass.AddColorImage(swapchain);
		renderPass.CreateRenderPass();
		auto info = Core::Instance::GetInfo();
		vpViews = vpRenderPass.GetView(2);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = Core::Instance::Get();
		init_info.PhysicalDevice = info.physicalDevice;
		init_info.Device = info.device;
		init_info.QueueFamily = info.queues.graphicsFamily.value();
		init_info.Queue = info.graphicsQueue;
		init_info.DescriptorPoolSize = 100;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		//TODO: understand why i need this here
		init_info.MinImageCount = info.MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = init_info.MinImageCount;
		init_info.RenderPass = renderPass.GetRenderPass();
		ImGui_ImplVulkan_Init(&init_info);

		ApplyTheme();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		grid = std::shared_ptr<Grid>(new Grid());
		grid->SetDrawOrder(1);
		grid->Create(*swapchain, vpRenderPass.GetRenderPass());
		grid->editorOnly = true;
		Scene::SceneTree::Current()->GetRoot()->AddChild(grid);
	}
	
	void Editor::AddTextures(vk::Sampler sampler) {
		this->sampler = sampler;
		for (auto i : vpViews.lock()->Views()) {
			renderedViewports.push_back(ImGui_ImplVulkan_AddTexture(sampler, i, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
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
		glm::vec2 cursor = Input::Mouse::CursorPos();
		ImGui::Text(std::format("Cursor position: X: {}, Y: {}", cursor.x, cursor.y).c_str());
		glm::vec2 delta = Input::Mouse::Delta();
		ImGui::Text(std::format("Cursor delta: X: {}, Y: {}", delta.x, delta.y).c_str());
		ImGui::End();

		sceneTree.Gui();
		inspector.Gui(sceneTree.GetSelected());

		ImGui::Render();
		
		cb.BeginPass(renderPass.GetRenderPass(), extent, renderPass.GetFramebuffer()[imageIndex]);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cb.GetBuffer());
		cb.EndPass();

		if (Input::Keyboard::IsDown(GLFW_KEY_LEFT_CONTROL) && Input::Keyboard::IsJustPressed(GLFW_KEY_S)) {
			serializer.SerializeTree(*Scene::SceneTree::Current());
		}
	}

	void Editor::Recreate() {
		for (auto i : renderedViewports) ImGui_ImplVulkan_RemoveTexture(i);
		renderedViewports.clear();
		renderPass.Recreate();
		AddTextures(sampler);
	}

	void Editor::Destroy() {
		for (auto i : renderedViewports) ImGui_ImplVulkan_RemoveTexture(i);
		renderPass.Destroy();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}