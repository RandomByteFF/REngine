#include "editor.hpp"

#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include "core/instance.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "scene/sceneTree.hpp"
#include "theme.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_structs.hpp"

namespace REngine::Editor {
	void Editor::Initialize(std::shared_ptr<Core::Swapchain> swapchain, Core::RenderPass vpRenderPass) {
		renderPass.AddColorAttachment().finalLayout = vk::ImageLayout::ePresentSrcKHR;
		renderPass.AddColorImage(swapchain);
		renderPass.CreateRenderPass();
		auto info = Core::Instance::GetInfo();
		vpViews = vpRenderPass.GetView(2);

		editorViewRP.AddColorAttachment().samples = Core::Instance::GetInfo().maxMsaa;
		editorViewRP.AddColorImage();
		editorViewRP.AddDepthAttachment().samples = Core::Instance::GetInfo().maxMsaa;
		editorViewRP.AddDepthImage();
		vk::AttachmentDescription &resolve = editorViewRP.AddResolveAttachment();
		resolve.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
		editorViewRP.AddResolveImage();

		editorViewRP.CreateRenderPass();

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

		barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
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
	}
	
	void Editor::AddTextures(vk::Sampler sampler) {
		this->sampler = sampler;
		for (auto i : vpViews.lock()->Views()) {
			renderedViewports.push_back(ImGui_ImplVulkan_AddTexture(sampler, i, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
		}
		for (auto i : editorViewRP.GetView(2).lock()->Views()) {
			renderedEditorViews.push_back(ImGui_ImplVulkan_AddTexture(sampler, i, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
		}
	}

	void Editor::Render(uint32_t imageIndex, Core::CommandBuffer cb, vk::Extent2D extent) {
		auto info = Core::Instance::GetInfo();
		cb.BeginPass(editorViewRP.GetRenderPass(), info.swapchainExtent, editorViewRP.GetFramebuffer()[info.currentFb]);
		Scene::SceneTree::Current()->CallDrawlist([&cb, this](Scene::Drawable &j) {
			j.DrawFromView(cb.GetBuffer(), *Scene::SceneTree::Current()->ActiveCamera());
		});
		cb.EndPass();

		barrier.image = editorViewRP.GetImage(2, info.currentFb);
		cb.GetBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(), nullptr, nullptr, barrier);


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
		ImGui::Begin("Player view");
		ImGui::Image(ImTextureID(VkDescriptorSet(renderedEditorViews[imageIndex])), ImGui::GetWindowSize());
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
		editorViewRP.Destroy();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}